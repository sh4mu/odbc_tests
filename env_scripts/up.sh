#!/bin/bash
IP=$1

if [ -z $IP ]
then
    echo "IP Missing"
    exit 1
fi

echo "Enable $IP"
iptables -D OUTPUT -d $IP -j DROP
iptables -D OUTPUT -d $IP -j ACCEPT