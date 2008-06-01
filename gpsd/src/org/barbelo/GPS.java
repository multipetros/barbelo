package org.barbelo;

import javax.microedition.location.*;
import java.util.*;

public class GPS implements LocationListener {
	private GPSd			_gpsd;
	private LocationProvider	_lp;

	public GPS(GPSd gpsd)
	{
		_gpsd = gpsd;

		try {
			Criteria cr = new Criteria();
			_lp = LocationProvider.getInstance(cr);
		} catch (Exception e) {
			_gpsd.exception(e);
		}
	}

	public void start()
	{
		_lp.setLocationListener(this, 1, 1, 1);
	}

	public void stop()
	{
		_lp.setLocationListener(null, -1, -1, -1);
	}

	public void locationUpdated(LocationProvider lp, Location location)
	{
		double lon = 0, lat = 0;

		if (location.isValid()) {
			QualifiedCoordinates c = 
				location.getQualifiedCoordinates();

			lon = c.getLongitude();
			lat = c.getLatitude();
		}

		String nmea = get_nmea(location);
		
		_gpsd.update_location(lon, lat, nmea);
	}

	private String get_nmea(Location location)
	{

		String nmea = location.getExtraInfo(
			"application/X-jsr179-location-nmea");

		if (nmea == null)
			return no_data();

		// N95 doesn't seem to put newlines after sentences
		StringBuffer p = new StringBuffer();
		int start = 0;

		for (int i = 1; i < nmea.length(); i++) {
			if (nmea.charAt(i) == '$') {
				p.append(nmea.substring(start, i));
				p.append("\r\n");
				start = i;
			}
		}
		p.append(nmea.substring(start));
		p.append("\r\n");

		return p.toString();
	}

	private String no_data()
	{
		StringBuffer p = new StringBuffer();

		p.append("$PBRB,0*");
		p.append(checksum(p));
		p.append("\r\n");

		return p.toString();
	}

	private String checksum(StringBuffer b)
	{
		char check = 0;

		for (int i = 1; i < (b.length() - 1); i++)
			check ^= b.charAt(i);

		String hex = Integer.toHexString(check);
		if (hex.length() < 2)
			hex = "0" + hex;

		return hex.toUpperCase();
	}

	public void providerStateChanged(LocationProvider lp, int newState)
	{
	}
}
