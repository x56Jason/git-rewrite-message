#!/bin/bash

Help()
{
   echo "Rewrite commit messages according to template"
   echo
   echo "Syntax:"
   echo "    git-rewrite-message.sh -t template-name <start>..HEAD"
   echo "    git-rewrite-message.sh -h"
   echo
}

while getopts ":ht:" option; do
   case $option in
      h) 
         Help
         exit;;
      t)
         template=$OPTARG;;
      *)
         echo "Error: Invalid option"
	 Help
         exit;;
   esac
done
shift $((OPTIND-1))

start=$1

export GIT_MSG_TEMPLATE=$template
export FILTER_BRANCH_SQUELCH_WARNING=1

echo
echo
echo "Rewrite commit messages: from $start to HEAD, using template: $template"
echo "==============================================================================="
echo

git filter-branch -f --msg-filter 'git-message-rewrite.pl' -- ${start}..HEAD
