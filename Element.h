/*
 * Element.h
 *
 *  Created on: Jan 15, 2020
 *      Author: STC-Design
 */

#ifndef ELEMENT_H_
#define ELEMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

namespace RubikBot{

    class Element {

    public:
        Element();		//default constructor
        Element(int indexX,int indexY, int color);
        virtual ~Element();

        void setIndex(int x, int y){indexx=x;indexy=y;};
        void setColor(int color){this->color=color;}
        void setMarking(int marking){this->marking=marking;}
        void recalcPos();
        int getValue(){return value;}
        void setValue(int value) {this->value=value;}
        int getFace(){return indexx;}           //where is it currently facing
        int getY(){return indexy;}
        int getX(){return indexx;}
        int getColor(){return color;}
        char getColorChar();
        int getMarking(){return marking;}
        bool inPosition();

    private:
        int indexx;
        int indexy;

        int color;
        int marking;
        int value;      //the value of the element which is [x*10]+[y]
                        //we will use this value to HUNT for a specific color; example EDGE WHITE GREEN is 47
    };
}

#ifdef __cplusplus
}
#endif


#endif /* ELEMENT_H_ */
