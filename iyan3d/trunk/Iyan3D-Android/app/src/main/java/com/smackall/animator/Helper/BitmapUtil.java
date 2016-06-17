package com.smackall.animator.Helper;

import android.graphics.Bitmap;
import android.graphics.Matrix;

import org.jcodec.common.model.ColorSpace;
import org.jcodec.common.model.Picture;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.IntBuffer;

/**
 * This class is part of JCodec ( www.jcodec.org ) This software is distributed
 * under FreeBSD License
 * 
 * @author The JCodec project
 * 
 */
public class BitmapUtil {
	private static ThreadLocal<int[]> buffer = new ThreadLocal<int[]>();

	public static Picture fromBitmap(Bitmap src) {
		if(src == null) return null;
		Picture dst = Picture.create((int) src.getWidth(),
				(int) src.getHeight(), ColorSpace.RGB);
		fromBitmap(src, dst);
		return dst;
	}

	public static void fromBitmap(Bitmap src, Picture dst) {
		int[] dstData = dst.getPlaneData(0);
		int[] packed = new int[src.getWidth() * src.getHeight()];

		src.getPixels(packed, 0, src.getWidth(), 0, 0, src.getWidth(),
				src.getHeight());

		for (int i = 0, srcOff = 0, dstOff = 0; i < src.getHeight(); i++) {
			for (int j = 0; j < src.getWidth(); j++, srcOff++, dstOff += 3) {
				int rgb = packed[srcOff];
				dstData[dstOff] = (rgb >> 16) & 0xff;
				dstData[dstOff + 1] = (rgb >> 8) & 0xff;
				dstData[dstOff + 2] = rgb & 0xff;
			}
		}
	}

	public static Bitmap toBitmap(Picture pic) {
		Bitmap dst = Bitmap.createBitmap(pic.getWidth(), pic.getHeight(),
				Bitmap.Config.ARGB_8888);
		toBitmap(pic, dst);
		return dst;
	}

	public static void toBitmap(Picture src, Bitmap dst) {
		int[] srcData = src.getPlaneData(0);
		int[] packed = getBuffer(src);

		for (int i = 0, dstOff = 0, srcOff = 0; i < src.getHeight(); i++) {
			for (int j = 0; j < src.getWidth(); j++, dstOff++, srcOff += 3) {
				packed[dstOff] = (srcData[srcOff] << 16)
						| (srcData[srcOff + 1] << 8) | srcData[srcOff + 2];
			}
		}
		dst.copyPixelsFromBuffer(IntBuffer.wrap(packed));
	}

	private static int[] getBuffer(Picture src) {
		int[] result = buffer.get();
		if (result == null) {
			result = new int[src.getWidth() * src.getHeight()];
			buffer.set(result);
		}
		return result;
	}

	public static void saveBitmap(Bitmap bmp, String path) {
		try {
			FileOutputStream fos = new FileOutputStream(path);
			bmp.compress(Bitmap.CompressFormat.JPEG, 100, fos);
			fos.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static Bitmap flip(Bitmap src) {
		Matrix matrix = new Matrix();
		matrix.preScale(1.0f, -1.0f);
		return Bitmap.createBitmap(src, 0, 0, src.getWidth(), src.getHeight(), matrix, true);
	}
}
