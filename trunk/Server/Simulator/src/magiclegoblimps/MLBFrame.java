/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package magiclegoblimps;

import java.awt.geom.Point2D.Double;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.util.*;
/**
 *
 * @author Kevin Hulin
 */
class MLBFrame extends JFrame{
    public final static int GRID_X = 10;
    public final static int GRID_Y = 8;
    public final static int SPACING = 60;
    public final static int TRIM = 20;
    public RobotMapPanel mp;
    public MLBController mc;
    public RobotPanel robotPanel;
    public ObjectPanel objectPanel;
    public GraphPanel graphPanel;
    public DemandPanel demandPanel;

    public MLBFrame(MLBController controller) {
        mc = controller;
        setPreferredSize(new Dimension(950, 800));
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLayout(new BorderLayout());
        JPanel center = new JPanel();
        center.setLayout(new BoxLayout(center,BoxLayout.PAGE_AXIS));
        mp = new RobotMapPanel(GRID_X, GRID_Y, SPACING);
        center.add(mp);
        graphPanel = new GraphPanel();
        //add(graphPanel,BorderLayout.CENTER);
       center.add(graphPanel);
       add(center,BorderLayout.CENTER);
        


        /*
        JPanel buttons = new JPanel(new GridLayout(0,2));
        JButton bLeft = new JButton("Left");
        JButton bRight = new JButton("Right");
        
        bLeft.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                for(MovableEntity o : mp.me){
                    if(o instanceof Robot)
                        ((Robot)o).nextLeft();
                }

            }
        });
        
        bRight.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                for(MovableEntity o : mp.me){
                    if(o instanceof Robot)
                        ((Robot)o).nextRight();
                }
            }
        });
        
        buttons.add(bLeft, 0);
        buttons.add(bRight, 1);
        add(buttons, BorderLayout.SOUTH);
        */
        robotPanel = new RobotPanel(mc);
        objectPanel = new ObjectPanel(mc);
        demandPanel = new DemandPanel(mc);

        JPanel controls = new JPanel();
        controls.setLayout(new BoxLayout(controls, BoxLayout.X_AXIS));
        controls.add(robotPanel);
        controls.add(objectPanel);
        controls.add(demandPanel);
        add(controls, BorderLayout.EAST);
        pack();
        setVisible(true);
    }

    public void update() {
        robotPanel.update();
        objectPanel.update();
        demandPanel.update();
    }
}
