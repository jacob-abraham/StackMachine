#!/usr/bin/env bash

cat grammar.txt | grammar_tool 3 >grammar_sets.txt
echo -e "\n" >> grammar_sets.txt
cat grammar.txt | grammar_tool 4 >>grammar_sets.txt