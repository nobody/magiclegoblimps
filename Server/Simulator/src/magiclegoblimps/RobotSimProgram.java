package magiclegoblimps;

import java.awt.geom.Point2D.Double;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.util.*;

class RobotSimProgram {
    MLBFrame mlbf;
    MLBController mlbc;
    public RobotSimProgram() { }

    public void start() {
        mlbc = new MLBController();
        mlbf = new MLBFrame(mlbc);
        mlbc.setFrame(mlbf);
        mlbc.runSimulation();
    }
}
