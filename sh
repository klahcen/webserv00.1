#!/bin/bash

commit_count_file=".commit_count.txt"

if [[ ! -f "$commit_count_file" ]]; then
  echo 1 > "$commit_count_file"
fi

count=$(< "$commit_count_file")

#while true; do ./sh; sleep 5; done

count=$((count + 1))

echo "$count" > "$commit_count_file"


if [[ 1 ]]; then
  git add .
  git commit -m "commit number $count"
  git push

  if [[ $? -ne 0 ]]; then
    echo "Error: Git operations failed."
    exit 1 
  fi

  echo "Commit pushed successfully!"
else
  echo "Aborted git add operation."
fi


echo "Recompiled successfull."