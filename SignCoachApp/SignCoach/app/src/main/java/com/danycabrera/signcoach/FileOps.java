package com.danycabrera.signcoach;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashSet;
import java.util.Locale;

public class FileOps {

	static String datasetPath = "signtalker/data/v1";

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

	// Copies XML dataset from the file assets to the device if not present in filesystem
	public static void copyDatasetFiles(Activity a) {
		try {
			File dir = new File(datasetPath);
			if (!dir.exists()) {
				Log.i("FileOps", "Copying dataset files into external storage directory.");
				FileOps.copyDirorfileFromAssetManager("data", datasetPath, a);
			} else {
				Log.i("FileOps", "Found dataset files in external storage directory.");
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static String copyDirorfileFromAssetManager(String arg_assetDir, String arg_destinationDir, Activity a) throws IOException {
		File sd_path = Environment.getExternalStorageDirectory();
		String dest_dir_path = sd_path + addLeadingSlash(arg_destinationDir);
		File dest_dir = new File(dest_dir_path);

		createDir(dest_dir);

		AssetManager asset_manager = a.getApplicationContext().getAssets();
		String[] files = asset_manager.list(arg_assetDir);

		for (int i = 0 ; i < files.length ; i++) {
			String abs_asset_file_path = addTrailingSlash(arg_assetDir) + files[i];
			String sub_files[] = asset_manager.list(abs_asset_file_path);

			if (sub_files.length == 0) {
				// It is a file
				String dest_file_path = addTrailingSlash(dest_dir_path) + files[i];
				copyAssetFile(abs_asset_file_path, dest_file_path, a);
			} else {
				// It is a sub directory
				copyDirorfileFromAssetManager(abs_asset_file_path, addTrailingSlash(arg_destinationDir) + files[i], a);
			}
		}

		return dest_dir_path;
	}


	public static void copyAssetFile(String assetFilePath, String destinationFilePath, Activity a) throws IOException {
		InputStream in = a.getApplicationContext().getAssets().open(assetFilePath);
		OutputStream out = new FileOutputStream(destinationFilePath);

		byte[] buf = new byte[1024];
		int len;
		while ((len = in.read(buf)) > 0)
			out.write(buf, 0, len);
		in.close();
		out.close();
	}

	public static String addTrailingSlash(String path) {
		if (path.charAt(path.length() - 1) != '/') {
			path += "/";
		}
		return path;
	}

	public static String addLeadingSlash(String path) {
		if (path.charAt(0) != '/') {
			path = "/" + path;
		}
		return path;
	}

	public static void createDir(File dir) throws IOException {
		if (dir.exists()) {
			if (!dir.isDirectory()) {
				throw new IOException("Can't create directory, a file is in the way");
			}
		} else {
			dir.mkdirs();
			if (!dir.isDirectory()) {
				throw new IOException("Unable to create directory");
			}
		}
	}
}
