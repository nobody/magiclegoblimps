package magiclegoblimps;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class ObjectPanel extends JPanel implements ActionListener
{
    JComboBox cmbObjects;
    JTextField txtName;
    JTextField txtRgbColor;
    JButton btnColor;
    JButton btnUpdate;
    JButton btnAdd;
    
    MLBController controller;
    
    public ObjectPanel(MLBController mlbc) {
        controller = mlbc;
        initComponents();
        buildGui();
    }
    
    public void actionPerformed(ActionEvent e) {
        String cmd = e.getActionCommand();
        if (cmd == "update") {
            if (cmbObjects.getSelectedIndex() < 2) {
                return;
            }
            ObjectOfInterest ooi = 
                (ObjectOfInterest)cmbObjects.getSelectedItem();
            String name = txtName.getText();
            Color color = txtName.getBackground();
            ooi.name = name;
            ooi.color = color;
            cmbObjects.setSelectedItem(ooi);
        } else if (cmd == "new object") {
            String name = txtName.getText();
            Color color = txtName.getBackground();
            ObjectOfInterest ooi = controller.addObject(name, color);
            cmbObjects.setSelectedItem(ooi);
        } else if (cmd == "select object") {
            if (cmbObjects.getSelectedIndex() < 0) {
                return;
            } else if (cmbObjects.getSelectedIndex() == 0) {
               txtName.setText("");
               txtName.setBackground(Color.WHITE); 
            } else {
               ObjectOfInterest ooi = 
                   (ObjectOfInterest)cmbObjects.getSelectedItem();
               txtName.setText(ooi.name);
               txtName.setBackground(ooi.color);
            }
        } else if (cmd == "choose color") {
            Color c = JColorChooser.showDialog(this, "Choose color",
                                               txtName.getBackground());
            txtName.setBackground(c);
        }
    }
    
    public void update() {
        cmbObjects.removeAllItems();
        cmbObjects.addItem("-- choose --");
        for (ObjectOfInterest o : controller.objects) {
            cmbObjects.addItem(o);
        }
        repaint();
    }
    
    private void initComponents() {
        cmbObjects = new JComboBox(controller.objects.toArray());
        cmbObjects.setActionCommand("select object");
        cmbObjects.addActionListener(this);
        
        txtName = new JTextField();
        
        btnColor = new JButton("Choose Color");
        btnColor.setActionCommand("choose color");
        btnColor.addActionListener(this);
        
        btnUpdate = new JButton("Update");
        btnUpdate.setActionCommand("update");
        btnUpdate.addActionListener(this);
        
        btnAdd = new JButton("New Object");
        btnAdd.setActionCommand("new object");
        btnAdd.addActionListener(this);
    }
    
    private void buildGui() {
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        add(new JLabel("Object:"));
        add(cmbObjects);
        add(new JLabel("Name:"));
        add(txtName);
        add(btnColor);
        add(btnUpdate);
        add(btnAdd);
    }
}
