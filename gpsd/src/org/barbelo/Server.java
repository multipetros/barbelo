package org.barbelo;

import javax.microedition.io.*;
import java.util.*;

public class Server extends Thread {
	private static final int PORT = 666;

	private GPSd			_gpsd;
	private boolean			_running;
	private ServerSocketConnection	_sock;
	private Vector			_clients;

	public Server(GPSd gpsd)
	{
		_gpsd    = gpsd;
		_running = true;
		_clients = new Vector();
	}

	private synchronized boolean running()
	{
		return _running;
	}

	public synchronized void stop()
	{
		_running = false;

		try {
			_sock.close();
		} catch (Exception e) {
		}

		for (int i = 0; i < _clients.size(); i++) {
			ClientHandler c = (ClientHandler) _clients.elementAt(i);

			c.kill();
		}
	}

	public void update_location(String nmea)
	{
		boolean recount = false;

		for (int i = 0; i < _clients.size(); i++) {
			ClientHandler c = (ClientHandler) _clients.elementAt(i);

			try {
				c.update_location(nmea);
			} catch (Exception e) {
				c.kill();
				_clients.removeElementAt(i);
				i--;
				recount = true;
			}
		}

		if (recount)
			client_count();
	}

	public void run()
	{
		try {
			_sock = (ServerSocketConnection) 
				Connector.open("socket://:" + PORT);
		} catch (Exception e) {
			_gpsd.exception(e);
			return;
		}

		while (running()) {
			run_do();
		}
	}

	private void run_do()
	{
		SocketConnection s = null;

		try {
			s = (SocketConnection) _sock.acceptAndOpen();
		} catch (Exception e) {
			return;
		}

		ClientHandler c = null;
		try {
			c = new ClientHandler(this, s);
		} catch (Exception e) {
			return;
		}

		_clients.addElement(c);
		client_count();
	}

	private void client_count()
	{
		int clients = _clients.size();

		_gpsd.set_clients(clients);
	}
}
