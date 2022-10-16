while [ 1 == 1 ]
do
  echo "26,1" > /proc/gpio_control
  sleep 0.2
  echo "26,0" > /proc/gpio_control
  sleep 0.2
done
