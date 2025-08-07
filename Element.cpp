/*
 * Element.cpp
 *
 *  Created on: Jan 15, 2020
 *      Author: STC-Design
 */

#include "Element.h"

namespace RubikBot{

    Element::Element() {
        // TODO Auto-generated constructor stub
        color=-1;
        indexx=-1;
        indexy=-1;
        marking=-1;
        value=-1;
    }

    Element::~Element() {
        // TODO Auto-generated destructor stub
    }


    Element::Element(int indexX, int indexY, int color)
    {

        indexx=indexX;
        indexy=indexY;
        this->color=color;

        //populate the value
        value=indexX*10+indexY;
    }

    /*
    void Element::setIndex(int x, int y)
    {
        indexx=x;
        indexy=y;
    }*/

    void Element::recalcPos()
    {
        //should not be used
    }

    bool Element::inPosition()
    {
        if (indexx*10 + indexy == getValue())
            return true;

        return false;
    }


}//namespace
