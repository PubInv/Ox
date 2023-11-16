#download  Ubuntu Server 22.04.3 LTS                                                       
#copy to usb                                                                     
#plug in keyboard , display and usb to sbc
#power up sbc                                       
#when splash screen hit F12                                                      
#boot to usb

#install ubuntu server                                                           
#   (you can choose minimal install)                                             
#   use entire disk                                                              
#     (you can turn off lvm - don't need it)                                     
#   install open ssh server                                                      
#   no need for any extra programs (at this time)                                
#   set username and password                                                     
#   add GitHub Keys to SSH account
#   let install finish                                                           
#   unplug usb flash drive
#   reboot                                                                       

#ssh into sbc   
#run the following
#sudo apt install git
#git clone https://github.com/PubInv/NASA-COG.git
#chmod +x 'SBC setup instructions'
#run ./NASA-COG/'SBC setup instructions'

#//install your favorite editor (sudo apt install emacs-nox)                 
#install tmux https://github.com/tmux/tmux/wiki                                          
#edit /etc/dpkg/dpkg.cfg.d/excludes                                              
sudo apt update
sudo apt dist-upgrade
sudo apt install -y avahi-daemon bash-completion emacs-nox nano vim less build-essential python3-venv git tmux

#   comment out the excludes for man and docs                                    
sudo apt install -y man-db manpages manpages-dev manpages-posix manpages-posix-dev                                                                            
sudo mv /usr/bin/man.REAL /usr/bin/man
sudo mandb -c

#use one of these
#wget https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -O get-platformio.py
curl -fsSL -o get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
python3 get-platformio.py
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
sudo mkdir -p /usr/local/bin
sudo ln -s ~/.platformio/penv/bin/platformio /usr/local/bin/platformio
sudo ln -s ~/.platformio/penv/bin/pio /usr/local/bin/pio
sudo ln -s ~/.platformio/penv/bin/piodebuggdb /usr/local/bin/piodebuggdb

