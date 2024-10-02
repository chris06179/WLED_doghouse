#pragma once

#include "wled.h"

//This is an empty v2 usermod template. Please see the file usermod_v2_example.h in the EXAMPLE_v2 usermod folder for documentation on the functions you can use!

class UsermodHeater : public Usermod {
  private:
    #define PinHeater 18 
    unsigned long lastCheck = 0;
    bool isHeating = false;
    int maxCount = 21600;
    int percentage = 100;
    int count = 0;    

    void setHeating(bool heat) {
      isHeating = heat;         
    }

    void startHeaterTime(int time) {
      this->maxCount = time;
      count = 0;
      setHeating(true);
    }
    

    void cancelHeating() {
      count = 0;
      digitalWrite(PinHeater, false);
      setHeating(false);
    }


  public:
    void setup() {
        pinMode(PinHeater, OUTPUT);
        setHeating(false);           
    }

    void loop() {

          
      if ((millis() - lastCheck >= 1000)) {            
        

        // Heizung ist an, max. Zeit überschritten
        if (isHeating && count >= maxCount) {
          cancelHeating();
        }

        // Rauch ist an, zählen
        if (isHeating) {
          count++;
          digitalWrite(PinHeater, second(localTime) < percentage * 0.6);  
        }    
        
        lastCheck = millis();        
      }     
      
      
    }

    /*
     * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void readFromJsonState(JsonObject& root)
    {

      bool resetHeater = root["heaterReset"];
      bool cancelHeater= root["heaterCancel"];
      int startHeater = root["heaterStart"];
      int setPercentage = root["heaterPercentage"];

      if (resetHeater == true) {
        count = 0;
      }

      if (cancelHeater == true) {
        this->cancelHeating();
      }

      if (startHeater > 0) {
        this->startHeaterTime(startHeater);
      }

      if (setPercentage > 0) {
        percentage = setPercentage;
        if (isHeating) {
          digitalWrite(PinHeater, second(localTime) < percentage * 0.6);
        }
      }

    }

     /*
     * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
     * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
     * Below it is shown how this could be used for e.g. a light sensor
     */
    
    void addToJsonInfo(JsonObject& root)
    {

      long heatRemaining = maxCount - count;

      JsonObject heater = root["heater"];
      if (heater.isNull()) heater = root.createNestedObject("heater");

      heater["count"] = count;  
      heater["maxCount"] = maxCount;
      heater["on"] = isHeating;
      heater["remaining"] = (heatRemaining < 0) ? 0 : heatRemaining;
      heater["percentage"] = percentage;
      
    }
};