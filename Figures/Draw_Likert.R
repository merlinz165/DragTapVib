require(grid)
require(lattice)
require(latticeExtra)
require(HH)
datafile <- read.csv('/Users/ting/WORKING/DragTapVib_Workspace/UserStudy/us_result_q_single.csv')
head(datafile)
likert(category ~. | question,
       subset(datafile, subtable=='pattern'),
       main=NULL,
       ylab=NULL,
       xlab="Participants",
       as.percent=F,
       layout=c(1,4))

multi_data <- read.csv('/Users/ting/WORKING/DragTapVib_Workspace/UserStudy/us_result_q_multi.csv')
likert(category ~. | question,
       subset(multi_data, subtable=='pattern'),
       main=NULL,
       ylab=NULL,
       xlab='Participants',
       ylim='',
       as.percent=F,
       layout=c(1,5))
