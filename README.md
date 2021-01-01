<div align="center">
  <h1>Message Queueing Service</h1>
  <p>
    <a href="#">
      <img src="https://img.shields.io/github/workflow/status/dvsku/ikp_projekat/build?color=%2331C653&logoColor=%23333A41&style=flat-square"/>
    </a>
  </p>
</div>
<br/><br/>

### Usage
```queueing_service.exe -s|-c <service_port> -p <clients_port>```
<br/>
### Usage examples<br/>
Starts queueing_service as host for another service on port 15000 and host for clients on port 15001.<br/>
```queueing_service.exe -s 15000 -p 15001``` <br/> <br/> 
Starts queueing_service as client and connects to another service on port 15000 and host for clients on port 15002.<br/>
```queueing_service.exe -s 15000 -p 15002``` <br/> 
### Authors
PR85/2016  Nikola Stojšin <br/>
PR116/2015 Sara Jović 
