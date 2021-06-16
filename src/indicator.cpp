/*
* Author: Wim Vissers
* The purpose of this library is to use a single LED as indicator.
*/
#include "Arduino.h"
#include "indicator.h"

Step::Step(const unsigned long duration, const byte state) : _duration(duration), _state(state)
{
    _next = NULL;
}

Step *Step::getNext()
{
    return this->_next;
}

void Step::setNext(Step *next)
{
    this->_next = next;
}

long Step::getDuration()
{
    return _duration;
}

byte Step::getState()
{
    return _state;
}

LEDMode::LEDMode(LEDMode *previous, const byte id) : _id(id), _theState(LOW)
{
    this->_first = NULL;
    this->_current = NULL;
    this->_previous = previous;
    this->_last = millis();
}

LEDMode::LEDMode(LEDMode *previous, const byte idleState, const byte id, const unsigned long msOn, const unsigned long msOff) : LEDMode(previous, id)
{
    addStep(new Step(msOn, idleState == LOW ? HIGH : LOW));
    addStep(new Step(msOff, idleState == LOW ? LOW : HIGH));
}

boolean LEDMode::execute(LEDProperties *led, const unsigned long now, STATE_CALLBACK_SIGNATURE)
{
    if (_current)
    {
        if ((now - _last) > _current->getDuration())
        {
            _current = _current->getNext();
            while (_current && (_current->getDuration() < 5))
            {
                _current = _current->getNext();
            }
            if (!_current)
            {
                _current = _first;
            }
            _last = now;
            //if (digitalRead(led->pin) != _current->getState())
            if (_theState != _current->getState())
            {
                _theState = _current->getState();
                if (callback)
                {
                    callback(_id, led->pin, _current->getState());
                }
#ifdef ESP8266
                if (led->analog)
                {
                    if (_current->getState() == led->idleState)
                    {
                        analogWrite(led->pin, 0);
                    }
                    else
                    {
                        analogWrite(led->pin, led->brightness);
                    }
                }
                else
                {

                    digitalWrite(led->pin, _current->getState());
                }
#else
                // ESP32 does not support analogWrite
                digitalWrite(led->pin, _current->getState());
#endif
            }
            return true;
        }
    }
    else if (_first)
    {
        _current = _first;
        _last = now;
        //if (digitalRead(led->pin) != _current->getState())
        if (_theState != _current->getState())
        {
            _theState = _current->getState();
            if (callback)
            {
                callback(_id, led->pin, _current->getState());
            }
#ifdef ESP8266
            if (led->analog)
            {
                if (_current->getState() == led->idleState)
                {
                    analogWrite(led->pin, 0);
                }
                else
                {
                    analogWrite(led->pin, led->brightness);
                }
            }
            else
            {
                digitalWrite(led->pin, _current->getState());
            }
#else
            digitalWrite(led->pin, _current->getState());
#endif
        }
        return true;
    }
    return false;
}

void LEDMode::addStep(Step *step)
{
    if (!_first)
    {
        _first = step;
    }
    if (_current)
    {
        _current->setNext(step);
    }
    _current = step;
}

void LEDMode::reset(const unsigned long now)
{
    _current = NULL;
    _last = 0;
    if (_first)
    {
        _theState = ~_first->getState();
    }
}

byte LEDMode::getId()
{
    return _id;
}

LEDMode *LEDMode::getPrevious()
{
    return _previous;
}

Indicator::Indicator(const byte ledPin) : Indicator(ledPin, LOW)
{
}

Indicator::Indicator(const byte ledPin, const byte idleState) : Indicator(ledPin, idleState, NULL)
{
}

Indicator::Indicator(const byte ledPin, const byte idleState, STATE_CALLBACK_SIGNATURE) : _currentMode(NULL), _ledModes(NULL), _enabled(true)
{
    this->_led.pin = ledPin;
    this->_led.idleState = idleState;
    this->_led.analog = false;
    this->_led.brightness = 500;
    this->callback = callback;
}

void Indicator::setCallback(STATE_CALLBACK_SIGNATURE)
{
    this->callback = callback;
}

boolean Indicator::begin()
{
    pinMode(_led.pin, OUTPUT);
    _last = millis();
    delay(500);
}

boolean Indicator::loop()
{
    if (_enabled)
    {
        unsigned long currentMillis = millis();
        if ((currentMillis - _last) > 100)
        {
            if (_currentMode != NULL)
            {
                _currentMode->execute(&_led, currentMillis, callback);
            }
            _last = currentMillis;
        }
    }
    return _enabled;
}

boolean Indicator::addMode(LEDMode *mode)
{
    _ledModes = mode;
    if (_currentMode == NULL)
    {
        _currentMode = mode;
    }
    return true;
}

boolean Indicator::addMode(const byte id, boolean on)
{
    LEDMode *mode = new LEDMode(_ledModes, id);
    byte state = _led.idleState == LOW ? on ? HIGH : LOW : on ? LOW
                                                              : HIGH;
    mode->addStep(new Step(1000, state));
    return this->addMode(mode);
}

boolean Indicator::addMode(const byte id, const unsigned long msShort, const unsigned long msLong, char *pattern)
{
    LEDMode *mode = new LEDMode(_ledModes, id);
    byte state = _led.idleState == LOW ? HIGH : LOW;
    int i = 0;
    char p = pattern[i];
    while (p != 0)
    {
        switch (p)
        {
        case 'A':
        case 'a':
        case 'L':
        case 'l':
        case '-':
            mode->addStep(new Step(msLong, state));
            break;
        default:
            mode->addStep(new Step(msShort, state));
        }
        i++;
        p = pattern[i];
        state = state == HIGH ? LOW : HIGH;
    }
    return this->addMode(mode);
}

boolean Indicator::addMode(const byte id, int size, const unsigned long sequence[])
{
    LEDMode *mode = new LEDMode(_ledModes, id);
    for (int i = 0; i < size; i++)
    {
        if ((i % 2) == 0)
        {
            mode->addStep(new Step(sequence[i], _led.idleState == LOW ? HIGH : LOW));
        }
        else
        {
            mode->addStep(new Step(sequence[i], _led.idleState == LOW ? LOW : HIGH));
        }
    }
    return this->addMode(mode);
}

boolean Indicator::addMode(const byte id, const unsigned long msOn, const unsigned long msOff)
{
    return addMode(id, msOn, msOff, "SL");
}

boolean Indicator::setMode(const byte id)
{
    LEDMode *mode = this->getMode(id);
    if (mode != NULL)
    {
        _currentMode = mode;
        _last = millis();
        mode->reset(_last);
    }
    return mode != NULL;
}

byte Indicator::getMode()
{
    return _currentMode->getId();
}

LEDMode *Indicator::getMode(const byte id)
{
    LEDMode *mode = _ledModes;
    while (mode && mode->getId() != id)
    {
        mode = mode->getPrevious();
    }
    return mode;
}

void Indicator::setEnabled(const boolean enabled)
{
    this->_enabled = enabled;
    digitalWrite(_led.pin, _led.idleState);
}

boolean Indicator::isEnabled()
{
    return _enabled;
}

void Indicator::setBrightness(int brightness)
{
    if (brightness > 0 && brightness <= 1024)
    {
        _led.brightness = brightness;
        _led.analog = true;
    }
    else
    {
        _led.brightness = 500;
        _led.analog = false;
    }
}
