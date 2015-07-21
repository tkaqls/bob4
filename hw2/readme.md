![Alt Text](https://github.com/tkaqls/bob4/blob/master/hw2/7.PNG)

![Alt Text](https://github.com/tkaqls/bob4/blob/master/hw2/6.PNG)

8kb 파일을 복사할 때는 mmio 쪽이 빠른 것을 확인할 수 있었습니다.


![Alt Text](https://github.com/tkaqls/bob4/blob/master/hw2/4.PNG)

32bit 운영체제에서는 최대 4GB 바이트의 메모리를 가질 수 있는데 한 번에 다룰 수 있는 메모리의 크기는 4GB 이기 때문에 5GB 파일을 MMIO를 통해서 생성할 수 없습니다. 

64bit 운영체제에서는 파일크기를 담고 있는 변수의 크기가 4바이트이기 때문에 4GB 이상을 포현할 수 없기 때문에 오류가 납니다. 

5GB 이상의 파일을 mmio를 통해서 만들려면 64bit 운영체제를 사용해서 파일의 크기를 8바이트 변수로 넘겨주거나 

32bit 운영체제에서는 크기를 나누어 오프셋을 계산하여 메모리를 매핑 후 해제 하는 형식으로 파일을 생성해야 합니다. 
