# vzmonitor
monitor/alert daemon for volkszaehler.org like sensors

## Features:
- define own "rules" as conditions incl. functions for:
  - comparision operators like >, <=,...
  - min
  - max
  - avg
  - lasttime
  - ...
- trigger an external script if a rule changes its status
- support for systemd watchdog

## Examples:
- check whether a sensor/channel is alive: (here at least one value per 10s)
```
{ "name": "ch1 dead?",
 "condition": {"LTE":[{"+":[10, {"LASTTIME":"ch1"}]},"NOW"]},
 "action": "./test.sh"
}
```
- real life example: check whether some water is constantly draining:
```
{ "name": "water draining?",
 "condition": {"GT":[{"MINVALUE":["water", 900]},0]},
 "action": "Check water! It's draining for more than 15mins!"
}
```
- for more see config file

## Command line options
Currently supported options:
- '-V' show version number and exit
- '-c' <file> file incl. path to config file.
- '-w' <seconds> interval for systemd watchdog seconds (if system supports sd_watchdog_enable it will be automatically detected and overwritten)
- '-h or -?' show help and exit.

## Config file options / format
see etc/vzmonitor.conf for an example

## To install:

clone to your local machine e.g.
```
git clone https://github.com/mbehr1/vzmonitor.git
```

make sure your gcc version is at least 4.7 e.g.
```
sudo apt-get install g++-4.7
rm /usr/bin/g++
cd /usr/bin
ln -s g++-4.7 /usr/bin/g++)
```

Preferraby (optional) install libsystemd-daemon-dev to have support for systemd watchdog:
```
sudo apt-get install libsystemd-daemon-dev
```

Have libmicrohttpd and libjson-c >= 0.12 installed (todo add howto/example)

```
cd vzmonitor
mkdir build
cmake ..
make && make test
sudo make install

copy and adjust systemd unit file from /etc to /etc/systemd/system/
copy and adjust config file from /etc to /etc/
sudo systemctl enable vzmonitor.service
sudo systemctl start vzmonitor.service
```
