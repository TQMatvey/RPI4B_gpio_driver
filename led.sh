while [ 1 == 1 ]
do
  echo "26,1" > /proc/gpio_control
  sleep 2
  echo "26,1" > /proc/gpio_control
  sleep 2
done