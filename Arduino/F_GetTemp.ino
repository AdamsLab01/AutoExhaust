void F_GetTemp() {
  float t = dht.readTemperature(true);
  float h = dht.readHumidity();
  
  temp = t;
  hum = h; 
}
