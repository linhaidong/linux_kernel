#########################################################################
# File Name: tag.sh
# Author: linhaidong
# Mail:   linhaidong@alibaba-inc.com
# Time:  二  9/11 23:01:22 2018
# Abstract: 
#########################################################################
#!/bin/sh
find `pwd` -name "*.h" -o -name "*.c" -o -name "*.cc" > cscope.files
cscope -bkq -i cscope.files
ctags -R
