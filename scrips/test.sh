#!/bin/bash
echo "auto config"
a=`expr $1 / 10`
command="remote_sensor"
ifconfig="ifconfig eth0 "
echo "remove firewall and iptables"
rm /etc/config/firewall
rm /etc/config/iptables
echo "remove rc.local"
rm /etc/rc.local
echo "Copy EXE to /usr/sbin"
cp remote_sensor /usr/sbin/
echo "Add +x "
chmod +x /usr/sbin/remote_sensor
cp "Sim900a_init.sh to /usr/sbin"
cp sim900a_init.sh /usr/sbin/
echo "add +x to sim900a_init.sh"
chmod +x /usr/sbin/sim900a_init.sh

channel_id=" -C "
PANID=" -P"
remote_address=" -R 192.168.0.5"
local_address=" -L 192.168.0."
ip=`expr 20 + $1`
local_address=${local_address}${ip}
if [ $a == "0"  ]
then
value=`expr $1 % 10 + 16`
channel_id=${channel_id}${value}
PANID=${PANID}" 4883"
elif [ $a == "1" ]
then
value=`expr $1 % 10 + 16`
channel_id=${channel_id}${value}
PANID=${PANID}" 8995"
elif [ $a == "2" ]
then
value=`expr $1 % 10 + 16`
channel_id=${channel_id}${value}
PANID=${PANID}" 13107"
fi
ipconfig= ${ifconfig}${local_address}
autorun= ${command}${channel_id}${PANID}${remote_address}${local_address}
echo "Create rc.local"
echo ipconfig > /etc/rc.local
echo autorun >> /etc/rc.local
echo "Config the net mode"
widora_mode router
sleep 30
echo "rebooting ...................................."
reboot
  
    
