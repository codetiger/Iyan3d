package com.smackall.animator.videokit.api.android;

import java.io.File;
import java.io.IOException;



import android.graphics.Bitmap;
import android.provider.Settings;
import android.util.Log;



import com.smackall.animator.videokit.scale.BitmapUtil;

import org.jcodec.common.model.Picture;


/**
 * This class is part of JCodec ( www.jcodec.org ) This software is distributed
 * under FreeBSD License
 * 
 * @author The JCodec project
 * 
 */
public class SequenceEncoder extends com.smackall.animator.videokit.jcodec.SequenceEncoder {

	public SequenceEncoder(File out) throws IOException {
		super(out);
	}

	public void encodeImage(Bitmap bi) throws IOException {

		Picture pic = BitmapUtil.fromBitmap(bi);
        encodeNativeFrame(pic);
    }
}