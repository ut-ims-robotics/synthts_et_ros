# **ROS wrapper for synths_et**
### Installimine:
sound_play on vaja eelnevalt installida

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

projekti kaustas jooksutada server vastavalt soovitavale häälele (kasutada eva.launch või tnu.launch):
```
source devel/setup.bash
roslaunch synthts_et_ros tnu.launch
```
uues terminalis liikuda projekti kausta ja jooksutada klient:
```
source devel/setup.bash
rosrun synthts_et_ros tekst_heliks_klient
```
