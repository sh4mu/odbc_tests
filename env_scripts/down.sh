#!/bin/bash
IP=$1

if [ -z $IP ]
then
    echo "IP Missing"
    exit 1
fi

echo "Disable $IP"
iptables -A OUTPUT -d $IP -j DROP