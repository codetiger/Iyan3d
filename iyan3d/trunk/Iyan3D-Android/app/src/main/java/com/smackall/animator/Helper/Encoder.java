package com.smackall.animator.Helper;

/**
 * Created by Sabish.M on 6/6/16.
 * Copyright (c) 2016 Smackall Games Pvt Ltd. All rights reserved.
 */

import org.jcodec.codecs.h264.H264Encoder;
import org.jcodec.codecs.h264.H264Utils;
import org.jcodec.common.FileChannelWrapper;
import org.jcodec.common.NIOUtils;
import org.jcodec.common.SeekableByteChannel;
import org.jcodec.common.model.ColorSpace;
import org.jcodec.common.model.Picture;
import org.jcodec.containers.mp4.Brand;
import org.jcodec.containers.mp4.MP4Packet;
import org.jcodec.containers.mp4.TrackType;
import org.jcodec.containers.mp4.muxer.FramesMP4MuxerTrack;
import org.jcodec.containers.mp4.muxer.MP4Muxer;
import org.jcodec.scale.ColorUtil;
import org.jcodec.scale.Transform;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;

public class Encoder {
    public SeekableByteChannel ch;
    public Picture toEncode;
    public Transform transform;
    public H264Encoder encoder;
    public ArrayList spsList;
    public ArrayList ppsList;
    public FramesMP4MuxerTrack outTrack;
    public ByteBuffer _out;
    public int frameNo;
    public MP4Muxer muxer;
    public FileOutputStream fileOutputStream;

    public Encoder(File out) throws IOException {
        fileOutputStream = new FileOutputStream(out);
        this.ch = new FileChannelWrapper(fileOutputStream.getChannel());
        this.muxer = new MP4Muxer(this.ch, Brand.MP4);
        this.outTrack = this.muxer.addTrack(TrackType.VIDEO, 25);
        this._out = ByteBuffer.allocate(12441600);
        this.encoder = new H264Encoder();
        this.transform = ColorUtil.getTransform(ColorSpace.RGB, this.encoder.getSupportedColorSpaces()[0]);
        this.spsList = new ArrayList();
        this.ppsList = new ArrayList();
    }

    public void encodeNativeFrame(Picture pic) throws IOException {
        if (this.toEncode == null) {
            this.toEncode = Picture.create(pic.getWidth(), pic.getHeight(), this.encoder.getSupportedColorSpaces()[0]);
        }

        this.transform.transform(pic, this.toEncode);
        this._out.clear();
        ByteBuffer result = this.encoder.encodeFrame(this.toEncode, this._out);
        this.spsList.clear();
        this.ppsList.clear();
        H264Utils.wipePS(result, this.spsList, this.ppsList);
        H264Utils.encodeMOVPacket(result);
        this.outTrack.addFrame(new MP4Packet(result, (long) this.frameNo, 25L, 1L, (long) this.frameNo, true, null, (long) this.frameNo, 0));
        ++this.frameNo;
    }

    public void finish() throws IOException {
        this.outTrack.addSampleEntry(H264Utils.createMOVSampleEntry(this.spsList, this.ppsList, 4));
        this.muxer.writeHeader();
        NIOUtils.closeQuietly(this.ch);
    }
}
