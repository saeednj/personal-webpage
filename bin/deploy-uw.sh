#!/bin/zsh

bundle exec jekyll build

cd _site

scp -r * snejati@ecelinux4.uwaterloo.ca:/web/snejati/public_html
