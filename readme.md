# **ROS wrapper for synths_et**
### Installimine:
valida kaust, kuhu projekt alla tõmmata
tekitada kausta src kaust
liikuda src kausta
jooksutada käsk: `git clone https://github.com/ut-ims-robotics/synthts_et_ros.git`
liikuda kausta synthts_et_ros
jooksutada käsud:
```
git submodule init
git submodule update
```
minna config kausta ning sealses parameetrid.yaml failis parameetrid vastavaks muuta
minna tagasi esimesse kausta
jooksutada `catkin_make`
### Kasutamine:
jooksutada: `roscore`
uues terminalis jooksutada: `rosrun sound_play soundplay_node.py`
uues terminalis liikuda projekti kausta ja jooksutada server:
```
source devel/setup.bash
roslaunch synthts_et_ros parameetrid.launch
```
uues terminalis liikuda projekti kausta ja jooksutada klient:
```
source devel/setup.bash
rosrun synthts_et_ros tekst_heliks_klient
```