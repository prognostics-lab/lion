#!/bin/bash

# Run tools from the root directory of the project
# TODO: Ignore TODOs in dependencies

echo -e "# TODO\nThis file contains a list of the TODO tasks found within the source code.\n\n" > TODO.md

patterns=$(grep -Er "[\/#]* TODO: ([\w\s]*)" ./*)

while read n; do
    filename=`echo $n | cut -d : -f 1`
    message=`echo $n | cut -d : -f 3`
    if [[ "$message" != ' ([ws]*)" ./*)' ]]; then
        echo -e "`cat TODO.md`\n- **(\`${filename}\`)**${message}" > TODO.md
    fi
done <<< "$patterns"
