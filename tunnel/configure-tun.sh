#!/bin/bash

ip addr add fc00:1234:ffff::1/64 dev $1
ip link set dev $1 up
ip link set dev $1 arp on