package org.barbelo;

import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;

public class GPSd extends MIDlet implements CommandListener {
	private Form		_form;
	private GPS		_gps;
	private StringItem	_error;
	private StringItem	_debug;
	private StringItem	_latitude;
	private StringItem	_longitude;
	private StringItem	_clients;
	private Server		_server;

	private static final String HIDE = "Hide";

	public GPSd()
	{
		_form = new Form("GPSd");
		_form.addCommand(new Command("Exit", Command.EXIT, 0));
		_form.addCommand(new Command(HIDE, Command.SCREEN, 0));
		_form.setCommandListener(this);

		_latitude	= new StringItem("Latitude", "");
		_longitude	= new StringItem("Longitude", "");
		_clients	= new StringItem("Clients", "0");
		_error		= new StringItem("Error", "none");
		_debug		= new StringItem("Debug", "");

		_form.append(_latitude);
		_form.append(_longitude);
		_form.append(_clients);
		_form.append(_error);
		_form.append(_debug);

		_gps    = new GPS(this);
		_server = new Server(this);
	}

	public void startApp()
	{
		Display.getDisplay(this).setCurrent(_form);

		_gps.start();
		_server.start();
	}

	public void pauseApp()
	{
	}

	public void destroyApp(boolean unconditional)
	{
	}

	public void commandAction(Command c, Displayable d)
	{
		if (c.getLabel().equals(HIDE)) {
			Display.getDisplay(this).setCurrent(null);
			return;
		}

		_gps.stop();

		_server.stop();
		_server.interrupt();
		try {
			_server.join();
		} catch (InterruptedException e) {
		}

		destroyApp(false);
		notifyDestroyed();
	}

	public void exception(Exception e)
	{
		_error.setText(e.toString());
	}

	public void debug(String msg)
	{
		_debug.setText(msg);
	}

	public void update_location(double latitude, double longitude,
				    String nmea)
	{
		_latitude.setText((new Double(latitude)).toString());
		_longitude.setText((new Double(longitude)).toString());

		_server.update_location(nmea);
	}

	public void set_clients(int clients)
	{
		_clients.setText((new Integer(clients)).toString());
	}
}
