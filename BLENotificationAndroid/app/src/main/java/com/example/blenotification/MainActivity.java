package com.example.blenotification;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.text.TextUtils;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MainActivity extends AppCompatActivity {
    public static final String TAG = "MainActivity";
    private static final String ENABLED_NOTIFICATION_LISTENERS = "enabled_notification_listeners";
    private static final String ACTION_NOTIFICATION_LISTENER_SETTINGS = "android.settings.ACTION_NOTIFICATION_LISTENER_SETTINGS";
    private static final int REQUEST_ENABLE_BT = 1;
    private static final long SCAN_PERIOD = 10000;
    private static final String BLE_DEVICE_NAME = "ESP32";
    private static final String BLE_DEVICE_UUID = "6E400001";
    private static final String BLE_CHARACTERISTIC_UUID = "6E400002";

    // Broadcast
    private NotificationPostedReceiver notificationPostedReceiver;

    // BLE
    private BluetoothAdapter mBluetoothAdapter;
    private BluetoothLeScanner mBluetoothLeScanner;
    private BluetoothGattCharacteristic btGattChar = null;

    private HashMap<String, BluetoothDevice> mDevices = new HashMap<>();
    private ArrayList<String> mAddresses = new ArrayList<>();
    private HashMap<String, BluetoothGatt> mBleGattHash = new HashMap<>();

    // Signal
    private byte dragSignal[] = {0x44};
    private byte tapSignal[] = {0x54};
    private byte vibSignal[] = {0x56};

    // UI
    private TextView logView;
    private Button connectButton;
    private Button disconnectButton;
    private Button scanButton;

    private static final class InterceptedNotificationCode {
        public static final int FACEBOOK_CODE = 1;
        public static final int GOOGLECHAT_CODE = 2;
        public static final int OTHER_NOTIFICATIONS_CODE = 4;
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // UI
        this.logView = findViewById(R.id.logView);
        this.logView.setMovementMethod(new ScrollingMovementMethod());

        checkBLESupport();
        checkPermissions();

        initScanButton();
        initConnectButton();
        initDisconnectButton();
        disableButtons();


        // Initializes Bluetooth adapter.
        final BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = null;
        if (bluetoothManager != null) {
            mBluetoothAdapter = bluetoothManager.getAdapter();
        } else {
            finish();
        }

        mBluetoothLeScanner = mBluetoothAdapter.getBluetoothLeScanner();

        // register a receive
        notificationPostedReceiver = new NotificationPostedReceiver();
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(String.valueOf(R.string.notification_intent_action));
        registerReceiver(notificationPostedReceiver, intentFilter);

    }

    @Override
    protected void onStart() {
        super.onStart();
        // Ensures Bluetooth is enabled on the device.  If Bluetooth is not currently enabled,
        // fire an intent to display a dialog asking the user to grant permission to enable it.
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        this.scanButton.setEnabled(true);
    }

    @Override
    protected void onPause() {

        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(notificationPostedReceiver);
    }

    private void sendNotification(int notificationCode) {
        if (notificationCode == InterceptedNotificationCode.FACEBOOK_CODE) {
            log("Got Notification from Facebook, activate Dragging.");
            sendData(dragSignal);
        } else if (notificationCode == InterceptedNotificationCode.GOOGLECHAT_CODE) {
            log("Got Notification from Google chat, activate Vibrating.");
            sendData(vibSignal);
        } else {
            log("Got Notification from other Apps, activate tapping.");
            sendData(tapSignal);
        }
    }

    public class NotificationPostedReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            int receivedNotificationCode = intent.getIntExtra("Notification Code", 0);
            sendNotification(receivedNotificationCode);
        }
    }

    // BLE
    private void initScanButton() {
        this.scanButton = findViewById(R.id.scanButton);
        this.scanButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                scanButton.setEnabled(false);
                scanDevices();
            }
        });
    }

    private void initConnectButton() {
        this.connectButton = findViewById(R.id.connectButton);
        this.connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                connectButton.setEnabled(false);
                if (!mBleGattHash.isEmpty()) {
                    disconnect();
                }
                for (String deviceAddress : mAddresses) {
                    log("Connecting to " + deviceAddress + "...");
                    connectToBLEDevice(deviceAddress);
                }

            }
        });
    }

    private void initDisconnectButton() {
        this.disconnectButton = findViewById(R.id.disconnectButton);
        this.disconnectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                disconnectButton.setEnabled(false);
                log("Disconnecting...");
                disconnect();
            }
        });
    }

    private void disableButtons() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                connectButton.setEnabled(false);
                disconnectButton.setEnabled(false);
            }
        });
    }

    private void checkBLESupport() {
        // Check if BLE is supported on the device.
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            log("BLE NOT SUPPORTED!");
            Toast.makeText(this, "BLE not supported!", Toast.LENGTH_SHORT).show();
            finish();
        }
    }

    private void checkPermissions() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {
            log("\"Access Fine Location\" permission not granted yet!");
            log("Whitout this permission Blutooth devices cannot be searched!");
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.ACCESS_FINE_LOCATION},
                    42);
        }
    }

    private void log(final String text) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                logView.setText(logView.getText() + "\n" + text);
            }
        });
    }


    private void scanDevices() {
        disconnect();
        this.mBluetoothLeScanner.stopScan(this.bleCallback);
        this.mDevices.clear();
        this.mAddresses.clear();
        this.mBluetoothLeScanner.startScan(this.bleCallback);
        // Search for 10 S
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                mBluetoothLeScanner.stopScan(bleCallback);
                if (!mAddresses.isEmpty()) {
                    connectButton.setEnabled(true);
                }
                scanButton.setEnabled(true);
            }
        }, SCAN_PERIOD);
    }

    private ScanCallback bleCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            BluetoothDevice device = result.getDevice();
            if(!mDevices.containsKey(device.getAddress()) && isThisTheDevice(device)) {
                deviceFound(device);
            }
        }

        @Override
        public void onBatchScanResults(List<ScanResult> results) {
            for(ScanResult sr : results) {
                BluetoothDevice device = sr.getDevice();
                if(!mDevices.containsKey(device.getAddress()) && isThisTheDevice(device)) {
                    deviceFound(device);
                }
            }
        }

        @Override
        public void onScanFailed(int errorCode) {
            Log.i("[BLE]", "scan failed with errorcode"  + errorCode);
        }
    };

    private boolean isThisTheDevice(BluetoothDevice device) {
        return null != device.getName() && device.getName().startsWith(BLE_DEVICE_NAME);
    }

    private void deviceFound(BluetoothDevice device) {
        this.mDevices.put(device.getAddress(), device);
        // TODO Multiple devices
        String deviceName = device.getName().trim();
        String deviceAddress = device.getAddress();
        if (!this.mAddresses.contains(deviceAddress)) {
            this.mAddresses.add(deviceAddress);
            log("Device " + deviceName + " found with address " + deviceAddress);
        }
    }

    private void connectToBLEDevice(String address) {
        BluetoothDevice device = this.mDevices.get(address);
        //this.mBluetoothLeScanner.stopScan(this.bleCallback);
        Log.i(TAG, "connect to device " + device.getAddress());
        BluetoothGatt bluetoothGatt = device.connectGatt(null, false, belConnectCallback);
        mBleGattHash.put(address, bluetoothGatt);
    }

    private final BluetoothGattCallback belConnectCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.i(TAG, "start service discovery " + gatt.discoverServices());
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                btGattChar = null;
                Log.i(TAG, "DisConnected with status " + status);
                fireDisconnected();
            } else {
                Log.i(TAG, "unknown state " + newState + " and status " + status);
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (null == btGattChar) {
                for (BluetoothGattService service : gatt.getServices()) {
                    if (service.getUuid().toString().toUpperCase().startsWith(BLE_DEVICE_UUID)) {
                        // TODO uuid
                        List<BluetoothGattCharacteristic> gattCharacteristics = service.getCharacteristics();
                        for (BluetoothGattCharacteristic bgc : gattCharacteristics) {
                            if (bgc.getUuid().toString().toUpperCase().startsWith(BLE_CHARACTERISTIC_UUID)) {
                                int chprop = bgc.getProperties();
                                if(((chprop & BluetoothGattCharacteristic.PROPERTY_WRITE) | (chprop & BluetoothGattCharacteristic.PROPERTY_WRITE_NO_RESPONSE)) > 0) {
                                    btGattChar = bgc;
                                    Log.i(TAG, "CONNECTED and ready to send");
                                    fireConnected();
                                }
                            }
                        }
                    }

                }
            }
        }
    };

    private void fireDisconnected() {
        log("[BLE]\tDisconnected");
        disableButtons();
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                connectButton.setEnabled(true);
            }
        });
    }

    private void fireConnected() {
        log("[BLE]\tConnected");
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                disconnectButton.setEnabled(true);
            }
        });
    }

    public void sendData(byte [] data) {
        this.btGattChar.setValue(data);
        String dataString = new String(data);

        log("Sending " + dataString + " to device.");

        for (Map.Entry mBleGatts : mBleGattHash.entrySet()) {
            BluetoothGatt bluetoothGatt = (BluetoothGatt) mBleGatts.getValue();
            bluetoothGatt.writeCharacteristic(this.btGattChar);
        }
    }

    public void disconnect() {
        for (Map.Entry mBleGatts : mBleGattHash.entrySet()) {
            BluetoothGatt bluetoothGatt = (BluetoothGatt) mBleGatts.getValue();
            bluetoothGatt.disconnect();
        }
        mBleGattHash.clear();
        disableButtons();
    }
}