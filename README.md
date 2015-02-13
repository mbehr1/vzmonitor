# vzmonitor
monitor/alert daemon for volkszaehler.org like sensors

To install:

clone to your local machine (e.g. git clone https://github.com/mbehr1/vzmonitor.git)

make sure your gcc version it at least 4.7 (sudo apt-get install g++-4.7; rm /usr/bin/g++; cd /usr/bin; rm /usr/bin/g++; ln -s g++-4.7 /usr/bin/g++)

cd vzmonitor

mkdir build

cmake ..

make && make test

sudo make install

copy and adjust systemd unit file from /etc to /etc/systemd/system/
copy and adjust config file from /etc to /etc/
sudo systemctl enable vzmonitor.service
sudo systemctl start vzmonitor.service
