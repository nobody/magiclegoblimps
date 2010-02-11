package magiclegoblimps;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class RobotPanel extends JPanel implements ActionListener
{
    JComboBox cmbRobots;
    JComboBox cmbObjects;
    JTextField txtName;
    JButton btnUpdate;
    JButton btnAdd;
    
    MLBController controller;
    
    public RobotPanel(MLBController mlbc) {
        controller = mlbc;
        initComponents();
        buildGui();
    }
    
    public void actionPerformed(ActionEvent e) {
        String cmd = e.getActionCommand();
        if (cmd == "update") {
            if (cmbRobots.getSelectedIndex() == 0) return;
            Robot r = (Robot)cmbRobots.getSelectedItem();
            r.name = txtName.getText();
            txtName.setText("");
            
            if (cmbObjects.getSelectedIndex() != 0) {
                ObjectOfInterest ooi =
                        (ObjectOfInterest)cmbObjects.getSelectedItem();
                controller.startTracking(r, ooi);
            } else {
                controller.stopTracking(r);
            }
            update();
        } else if (cmd == "new robot") {
            ObjectOfInterest ooi = null;
            if (cmbObjects.getSelectedIndex() > 0) {
                ooi = (ObjectOfInterest)cmbObjects.getSelectedItem();
            }
            
            Robot r = controller.addRobot(txtName.getText());
            controller.startTracking(r, ooi);
            cmbRobots.setSelectedItem(r);
        } else if (cmd == "select robot") {
            if (cmbRobots.getSelectedIndex() < 0) {
                txtName.setText("");
            } else if (cmbRobots.getSelectedIndex() == 0) {
                txtName.setText("");
                if (cmbObjects.getSelectedIndex() >= 0) {
                    cmbObjects.setSelectedIndex(0);
                }
            } else {
                Robot r = (Robot)cmbRobots.getSelectedItem();
                txtName.setText(r.toString());
                if (controller.isTracking(r)) {
                    cmbObjects.setSelectedItem(controller.getTrackingObject(r));
                } else if (cmbObjects.getSelectedIndex() >= 0) {
                    cmbObjects.setSelectedIndex(0);
                }
            }
        }
    }
    
    public void update() {
        cmbRobots.removeAllItems();
        cmbRobots.addItem("-- choose --");
        for (Robot r : controller.robots) {
            cmbRobots.addItem(r);
        }
        
        cmbObjects.removeAllItems();
        cmbObjects.addItem("-- choose --");
        for (ObjectOfInterest o : controller.objects) {
            cmbObjects.addItem(o);
        }
        repaint();
    }
    
    private void initComponents() {
        cmbRobots = new JComboBox(controller.robots.toArray());
        cmbRobots.setActionCommand("select robot");
        cmbRobots.addActionListener(this);
        
        cmbObjects = new JComboBox(controller.objects.toArray());
        
        txtName = new JTextField();
        
        btnUpdate = new JButton("Update");
        btnUpdate.setActionCommand("update");
        btnUpdate.addActionListener(this);
        
        btnAdd = new JButton("New Robot");
        btnAdd.setActionCommand("new robot");
        btnAdd.addActionListener(this);
    }
    
    private void buildGui() {
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        add(new JLabel("Robot:"));
        add(cmbRobots);
        add(new JLabel("Name:"));
        add(txtName);
        add(new JLabel("Track object:"));
        add(cmbObjects);
        add(btnUpdate);
        add(btnAdd);
    }
}
