package org.barbelo;

import javax.microedition.io.*;
import java.io.*;

public class ClientHandler {
	private Server			_server;
	private SocketConnection	_s;
	private DataOutputStream	_os;

	public ClientHandler(Server server, SocketConnection s) throws Exception
	{
		_server = server;
		_s      = s;
		_os     = _s.openDataOutputStream();
	}

	public synchronized void kill()
	{
		try {
			_os.close();
			_s.close();
			_os = null;
		} catch (Exception e) {
		}
	}

	public synchronized void update_location(String nmea) throws Exception
	{
		byte[] crap = nmea.getBytes("UTF-8");
		_os.write(crap);
		_os.flush();
	}
}
