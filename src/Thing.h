/*
|| Thing.h - Library to interface with the Mozilla Things Gateway.
||
||
|| More notes at the bottom.
*/

#pragma once

#include <Arduino.h>

enum ThingPropertyDatatype
{
  BOOLEAN             = 0x00,
  NUMBER              = 0x01,
  STRING              = 0x02
// BYTEARRAY -- coming soon
};

enum ThingType
{
  THING               = 0x00,
  ONOFFSWITCH         = 0x01,
  MULTILEVELSWITCH    = 0x02,
  BINARYSENSOR        = 0x03,
  MULTILEVELSENSOR    = 0x04,
  SMARTPLUG           = 0x05
};


// Common base class for all properties
class ThingProperty
{
  public:
    ThingProperty *next = nullptr;  // linked list pointer
    // TODO: accept PGMSPACE pointers too, if on AVR 8 bit
    char *name = nullptr;  // TODO: name must be unique, since gateway indexes thing properties in a Map, using name as the key
    char *description = nullptr;
    boolean changed = false;
    ThingPropertyDatatype type = NUMBER;

    // TODO: Virtuals for getValue() & setValue()

    // constructor
    ThingProperty(const char *propertyName, const char *propertyDescription, ThingPropertyDatatype propertyDatatype)
      : name(propertyName),
        description(propertyDescription),
        type(propertyDatatype)
    {
    }
};


class ThingPropertyBoolean : public ThingProperty
{
  public:
    boolean value;

    ThingPropertyBoolean(const char *propertyName, const char *propertyDescription)
      : ThingProperty(propertyName, propertyDescription, BOOLEAN)
    {
    }

    void setValue(boolean newValue)
    {
      if (newValue != this->value)
      {
        this->value = newValue;
        this->changed = true;
      }
    }

    boolean getValue()
    {
      return this->value;
    }

    // TODO: add operator overloads?
};


class ThingPropertyNumber : public ThingProperty
{
  public:
    int32_t value;

    ThingPropertyNumber(const char *propertyName, const char *propertyDescription)
      : ThingProperty(propertyName, propertyDescription, NUMBER)
    {
    }

    void setValue(int32_t newValue)
    {
      if (newValue != this->value)
      {
        this->value = newValue;
        this->changed = true;
      }
    }

    int32_t getValue()
    {
      return this->value;
    }

    // TODO: add operator overloads?
};


// TODO: um... yeah
class ThingPropertyNumberRange : public ThingPropertyNumber
{
  public:
    int32_t minimum;
    int32_t maximum;
};


class ThingPropertyString : public ThingProperty
{
  public:
    // TODO: fix restriction of max 32 bytes per STRING (including terminator) -- template? String type?
    const uint8_t maxLength = 32;
    char value[32];

    ThingPropertyString(const char *propertyName, const char *propertyDescription)
      : ThingProperty(propertyName, propertyDescription, STRING)
    {
    }

    void setValue(const char *newValue)
    {
      if (newValue != nullptr)
      {
        if (strcmp(newValue, this->value) != 0)
        {
          // Copy in as much as we can
          strncpy(this->value, newValue, this->maxLength - 1);
          this->changed = true;
        }
      }
    }

    char *getValue()
    {
      return this->value;
    }

    // TODO: add operator overloads?
};


class ThingDevice
{
  public:
    char *name = nullptr;
    char *description = nullptr;
    ThingType type = THING;
    ThingDevice *next = nullptr;
    uint8_t propertyCount = 0;
    uint8_t eventCount = 0;
    uint8_t actionCount = 0;
    ThingProperty *firstProperty = nullptr;
    ThingProperty *lastProperty = nullptr;
    // TODO: implement actions & events
    //ThingEvent *firstEvent = nullptr;
    //ThingEvent *lastEvent = nullptr;
    //ThingAction *firstAction = nullptr;
    //ThingAction *lastAction = nullptr;

    ThingDevice(const char *deviceName, const char *deviceDescription, ThingType deviceType)
      : name(deviceName),
        description(deviceDescription),
        type(deviceType)
    {
    }

    uint8_t addProperty(ThingProperty &property)
    {
      if (this->lastProperty == nullptr)
      {
        this->firstProperty = &property;
        this->lastProperty = &property;
      }
      else
      {
        this->lastProperty->next = &property;
        this->lastProperty = &property;
      }
      this->propertyCount++;

      return this->propertyCount;
    }

    ThingProperty *getProperty(uint8_t propertyIdx)
    {
      if (propertyIdx >= this->propertyCount)
        return nullptr;     // OOPS!  Index out of range

      ThingProperty *property = this->firstProperty;

      for (int propertyNum = 0; propertyNum < propertyIdx; propertyNum++)
      {
        property = property->next;
        if (property == nullptr)
          return nullptr;   // OOPS OOPS! A nullptr where there shouldn't be?
      }

      return property;
    }
};


class ThingAdapter
{
  private:
  protected:
  public:
    char *name;
    char *description;
    uint8_t thingCount = 0;
    ThingDevice *firstDevice = nullptr;
    ThingDevice *lastDevice = nullptr;

//    virtual void enumerateAdapter() = 0;
//    virtual void enumerateThings() = 0;
//    virtual void enumerateProperties() = 0;

    // Constructor
    ThingAdapter(const char *adapterName, const char *adapterDescription)
      : name(adapterName),
        description(adapterDescription)
    {
    }

    uint8_t addDevice(ThingDevice &device) 
    {
      if (this->lastDevice == nullptr)
      {
        this->firstDevice = &device;
        this->lastDevice = &device;
      }
      else
      {
        this->lastDevice->next = &device;
        this->lastDevice = &device;
      }
      this->thingCount++;

      return this->thingCount;
    }

    ThingDevice *getDevice(uint8_t thingIdx)
    {
      if (thingIdx >= this->thingCount)
        return nullptr;     // OOPS!  Index out of range

      ThingDevice *device = this->firstDevice;

      for (int deviceNum = 0; deviceNum < thingIdx; deviceNum++)
      {
        device = device->next;
        if (device == nullptr)
          return nullptr;   // OOPS OOPS! A nullptr where there shouldn't be?
      }

      return device;
    }
};


// TODO: ThingEvent, ThingAction, units for number properties


/*
||
|| @author         Brett Hagman <bhagman@roguerobotics.com>
|| @url            http://roguerobotics.com/
|| @url            http://oryng.org/
||
|| @description
|| | Thing Library
|| |
|| |
|| #
||
|| @notes
|| |
|| |
|| |
|| #
||
|| @license Please see LICENSE.
||
*/
