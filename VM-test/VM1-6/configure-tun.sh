#!/bin/bash

ip addr add fc00:1234:ffff::1/64 dev tun0
ip link set dev tun0 up
ip link set dev tun0 arp on