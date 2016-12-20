package com.danycabrera.signcoach;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashSet;
import java.util.Locale;

public class FileOps {

	static File externalStoragePath;
	static String externalAppPath = "signcoach"; // Relative path to app files folder in external storage
	static String datasetPath = "data/v1"; // Relative path to dataset in assets

	private static final int REQUEST_EXTERNAL_STORAGE = 1;
	private static String[] PERMISSIONS_STORAGE = {
			Manifest.permission.READ_EXTERNAL_STORAGE,
			Manifest.permission.WRITE_EXTERNAL_STORAGE
	};

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


	// Sets external storage path (finishes with a "/"
	// Creates app files directory if non-existent
	// Verifies storage permissions and returns true if we have them (if not, we won't be able to access the file system)
	public static boolean init(Activity act) {
		try {
			externalStoragePath = Environment.getExternalStorageDirectory();

			if (!FileOps.verifyPermissions(act)) {
				return false;
			}

			File dir = new File(externalStoragePath + "/" + externalAppPath + "/");
			createDir(dir);
		} catch (Exception e) {
			Log.e("FileOps", "Unable to initialize FileOps.");
			e.printStackTrace();
			return false;
		}

		return true;
	}

	// Returns the full external app path
	public static String getAppPath() {
		return externalStoragePath + "/" + externalAppPath;
	}

	// Copies XML dataset from the file assets to the device if not present in filesystem
	// Returns true on success
	public static boolean copyDatasetFiles(Activity a) {
		try {
			String path = externalAppPath + "/" + datasetPath;
			File dir = new File(externalStoragePath + "/" + path);

			if (!dir.exists()) {
				FileOps.copyDirorfileFromAssetManager(datasetPath, path, a);
			} else {
				Log.i("FileOps", "Found dataset files in external storage directory " + dir.toString());
			}
		} catch (IOException e) {
			Log.e("FileOps", "Unable to copy dataset files to phone storage.");
			e.printStackTrace();
			return false;
		}

		return true;
	}

	public static String copyDirorfileFromAssetManager(String arg_assetDir, String arg_destinationDir, Activity a) throws IOException {
		String dest_dir_path = externalStoragePath + addLeadingSlash(arg_destinationDir);
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

	// Tries to create a directory in the path provided. Can create sub-directories as well.
	public static void createDir(File dir) throws IOException {
		if (dir.exists()) {
			if (!dir.isDirectory()) {
				throw new IOException("Can't create directory, a file is in the way");
			}
		} else {
			try {
				Log.i("FileOps", "createDir() " + dir.toString());
				dir.mkdirs();
			}  catch (Exception e) {
				Log.e("FileOps", "Unable to create directory " + dir.toString());
				e.printStackTrace();
			}

			if (!dir.isDirectory()) {
				throw new IOException("Unable to create directory " + dir.toString());
			}
		}
	}

	// returns true if we have file permissions
	// if not and Android_6+, show the request permission dialog
	public static boolean verifyPermissions(Activity activity) {
		int permission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_EXTERNAL_STORAGE);

		if (permission != PackageManager.PERMISSION_GRANTED) {
			// We don't have permission so prompt the user
			Log.e("FileOps", "No permissions for external storage. I will try to get them from the user (Android 6+).");
			ActivityCompat.requestPermissions(
					activity,
					PERMISSIONS_STORAGE,
					REQUEST_EXTERNAL_STORAGE
			);
		}

		return true;
	}

	// Returns the full dataset path, supposing the object has been initialized
	public static String getDatasetPath() {
		return externalStoragePath + "/" +  externalAppPath + "/" + datasetPath;
	}
}
