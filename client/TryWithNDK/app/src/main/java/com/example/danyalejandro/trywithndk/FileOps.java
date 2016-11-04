package com.example.danyalejandro.trywithndk;

import android.util.Log;

import java.io.InputStream;
import java.util.HashSet;
import java.util.Locale;

public class FileOps {

	// Returns a hashset of strings with all the mounted storage paths
	public static HashSet<String> getExternalMounts() {
		final HashSet<String> out = new HashSet<String>();
		String reg = "(?i).*vold.*(vfat|ntfs|exfat|fat32|ext3|ext4).*rw.*";
		String s = "";
		try {
			final Process process = new ProcessBuilder().command("mount")
					.redirectErrorStream(true).start();
			process.waitFor();
			final InputStream is = process.getInputStream();
			final byte[] buffer = new byte[1024];
			while (is.read(buffer) != -1) {
				s = s + new String(buffer);
			}
			is.close();
		} catch (final Exception e) {
			e.printStackTrace();
		}

		// parse output
		final String[] lines = s.split("\n");
		for (String line : lines) {
			if (!line.toLowerCase(Locale.US).contains("asec")) {
				if (line.matches(reg)) {
					String[] parts = line.split(" ");
					for (String part : parts) {
						if (part.startsWith("/"))
							if (!part.toLowerCase(Locale.US).contains("vold"))
								out.add(part);
					}
				}
			}
		}
		return out;
	}

	// Returns the external storage path or an empty string if none found
	public static String getStoragePath() {
		String result = "";
		HashSet<String> mounts = getExternalMounts();
		for (String s : mounts) {
			result = s;
		}

		return result;
	}
}
