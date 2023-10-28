#pragma once
#include <iostream>
#include <string>
#include <utility>
#include <cmath>
#include <algorithm>
#include <optional>

// defining region codes
enum OutCode {
    INSIDE = 0b0000,
    LEFT = 0b0001,
    RIGHT = 0b0010,
    BOTTOM = 0b0100,
    TOP = 0b1000
};

int compute_outcode(double x, double y, int x_min, int y_min, int x_max, int y_max) {
    // initialized as being inside
    int code = INSIDE;

    // to the left of rectangle
    if (x < x_min) {
        code |= LEFT;
    // to the right of rectangle
    } else if (x > x_max) {
        code |= RIGHT;
    }

    // below the rectangle
    if (y < y_min) {
        code |= BOTTOM;
    // above the rectangle
    } else if (y > y_max) {
        code |= TOP;
    }
 
    return code;
}

std::optional<std::tuple<int,int,int,int>> cohen_sutherland_clip(int x1, int y1, int x2, int y2, int x_min, int y_min, int x_max, int y_max) {

    // compute region codes for P1, P2
    int code1 = compute_outcode(x1, y1, x_min, y_min, x_max, y_max);
    int code2 = compute_outcode(x2, y2, x_min, y_min, x_max, y_max);
 
    // initialize line as outside the rectangular window
    bool accept = false;
 
    while (true) {
        if ((code1 == 0) && (code2 == 0)) {
            // if both endpoints lie within rectangle
            accept = true;
            break;
        } else if (code1 & code2) {
            // if both endpoints are outside rectangle in same region
            break;
        } else {
            // some segment of line lies within the rectangle
            int code_out;
            int x, y;
 
            // At least one endpoint is outside the rectangle, pick it.
            if (code1 != 0) {
                code_out = code1;
            } else {
                code_out = code2;
            }
 
            // find intersection point;
            // using formulas y = y1 + slope * (x - x1),
            // x = x1 + (1 / slope) * (y - y1)
            if (code_out & TOP) {
                // point is above the clip rectangle
                x = x1 + (x2 - x1) * (y_max - y1) / (y2 - y1);
                y = y_max;
            } else if (code_out & BOTTOM) {
                // point is below the rectangle
                x = x1 + (x2 - x1) * (y_min - y1) / (y2 - y1);
                y = y_min;
            } else if (code_out & RIGHT) {
                // point is to the right of rectangle
                y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1);
                x = x_max;
            } else if (code_out & LEFT) {
                // point is to the left of rectangle
                y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1);
                x = x_min;
            }
 
            // now intersection point x, y is found we replace point outside rectangle
            // ... by intersection point
            if (code_out == code1) {
                x1 = x;
                y1 = y;
                code1 = compute_outcode(x1, y1, x_min, y_min, x_max, y_max);
            } else {
                x2 = x;
                y2 = y;
                code2 = compute_outcode(x2, y2, x_min, y_min, x_max, y_max);
            }
        }
    }

    if (accept) {
        return std::optional<std::tuple<int,int,int,int>>(std::make_tuple(x1,y1,x2,y2));
    } else {
        return std::optional<std::tuple<int,int,int,int>>();
    }
}
