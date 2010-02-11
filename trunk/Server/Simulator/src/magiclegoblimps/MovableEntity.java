package magiclegoblimps;

import java.awt.geom.Point2D.Double;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.util.*;

abstract class MovableEntity {
    Point2D.Double pos;
    double dir = 0;
    double speed = 0;
    String name = "DEFAULT_NAME";
    public abstract void move();
    abstract void draw(Graphics g);
    
    public String toString() {
        return name;
    }
}
