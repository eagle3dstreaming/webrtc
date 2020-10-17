/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

package org.webrtc;

import android.content.Context;
import android.support.annotation.Nullable;
import org.webrtc.DefaultVideoEncoderFactory;
import org.webrtc.DefaultVideoDecoderFactory;

/*
import java.nio.ByteBuffer;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.TimeUnit;
*/

public class UnityUtility {
  private static final String VIDEO_CAPTURER_THREAD_NAME = "VideoCapturerThread";

  private static final boolean ENABLE_H264_HIGH_PROFILE = false;

  private static EglBase rootEglBase = EglBase.create();;

  public static SurfaceTextureHelper LoadSurfaceTextureHelper() {
    final SurfaceTextureHelper surfaceTextureHelper =
        SurfaceTextureHelper.create(VIDEO_CAPTURER_THREAD_NAME, null);
    return surfaceTextureHelper;
  }

  private static boolean useCamera2() {
    return Camera2Enumerator.isSupported(ContextUtils.getApplicationContext());
  }


//// TEST Probe Begin -- it will be disable when we integerate in worklink

/*
  public static class MyVideoCapturer implements VideoCapturer {

    private static final int frameWidth = 720;
    private static final int frameHeight = 480;


    public VideoFrame getNextFrame() {
      final long captureTimeNs = TimeUnit.MILLISECONDS.toNanos(SystemClock.elapsedRealtime());
      final JavaI420Buffer buffer = JavaI420Buffer.allocate(frameWidth, frameHeight);
      final ByteBuffer dataY = buffer.getDataY();
      final ByteBuffer dataU = buffer.getDataU();
      final ByteBuffer dataV = buffer.getDataV();
      final int chromaHeight = (frameHeight + 1) / 2;
      final int sizeY = frameHeight * buffer.getStrideY();
      final int sizeU = chromaHeight * buffer.getStrideU();
      final int sizeV = chromaHeight * buffer.getStrideV();


      String str = "Arvind";
      byte[] byteArr = str.getBytes();

      //  buffer.setAugData(byteArr);
      // buffer.setAugLen(str.length());
      return new VideoFrame(buffer, 0 , captureTimeNs);
    }

    public void PushVideoFrame(VideoFrame videoFrame, byte[] SerializedCameraData, int length)
    {
      capturerObserver.onFrameCaptured(videoFrame);
    }



    private final static String TAG = "MyVideoCapturer";
    private CapturerObserver capturerObserver;
    private final Timer timer = new Timer();

    private final TimerTask tickTask = new TimerTask() {
      @Override
      public void run() {
        tick();
      }
    };


    public void tick() {
      VideoFrame videoFrame = getNextFrame();

      String str = "ArvindUmrao";
      byte[] byteArr = str.getBytes();

      PushVideoFrame(videoFrame, byteArr, str.length() );

      //capturerObserver.onFrameCaptured(videoFrame);
      videoFrame.release();
    }

    @Override
    public void initialize(SurfaceTextureHelper surfaceTextureHelper, Context applicationContext,
                           CapturerObserver capturerObserver) {
      this.capturerObserver = capturerObserver;
    }

    @Override
    public void startCapture(int width, int height, int framerate) {
      timer.schedule(tickTask, 0, 1000 / framerate);
    }

    @Override
    public void stopCapture() throws InterruptedException {
      timer.cancel();
    }

    @Override
    public void changeCaptureFormat(int width, int height, int framerate) {
      // Empty on purpose
    }

    @Override
    public void dispose() {

    }

    @Override
    public boolean isScreencast() {
      return false;
    }

  };
*/
  // TEST Probe End -- it will be disable when we integerate in worklink

  private static @Nullable VideoCapturer createCameraCapturer(CameraEnumerator enumerator) {
    final String[] deviceNames = enumerator.getDeviceNames();

    for (String deviceName : deviceNames) {
      if (enumerator.isFrontFacing(deviceName)) {
        VideoCapturer videoCapturer = enumerator.createCapturer(deviceName, null);

        if (videoCapturer != null) {
          return videoCapturer;
        }
      }
    }

    return null;
  }

// arvind end

  public static VideoEncoderFactory  MultiplexH264Encoder(  )
  {
      VideoEncoderFactory encoderFactory = new MultiplexVideoEncoderFactory(null, ENABLE_H264_HIGH_PROFILE);

      return encoderFactory;
  }

    public static VideoDecoderFactory  MultiplexH264Decoder(  )
    {
        VideoDecoderFactory decoderFactory = new MultiplexVideoDecoderFactory(null);

        return decoderFactory;
    }

  public static VideoEncoderFactory  DefaultVideoEncoderFactory(  )
  {
      VideoEncoderFactory encoderFactory =  new DefaultVideoEncoderFactory(
              rootEglBase.getEglBaseContext(),  /* enableIntelVp8Encoder */true,  /* enableH264HighProfile */false);
      return encoderFactory;
  }

  public static VideoDecoderFactory  DefaultVideoDecoderFactory(  )
  {

     VideoDecoderFactory decoderFactory = new DefaultVideoDecoderFactory(rootEglBase.getEglBaseContext());
     return decoderFactory;
  }



  /*
  public static VideoCapturer LinkCamera(
      long nativeTrackSource, SurfaceTextureHelper surfaceTextureHelper) {
    VideoCapturer capturer = new MyVideoCapturer();
        //createCameraCapturer(new Camera2Enumerator(ContextUtils.getApplicationContext()));

    VideoSource videoSource = new VideoSource(nativeTrackSource);

    capturer.initialize(surfaceTextureHelper, ContextUtils.getApplicationContext(),
        videoSource.getCapturerObserver());

    capturer.startCapture(720, 480, 30);
    return capturer;
  }
*/
  public static void StopCamera(VideoCapturer camera) throws InterruptedException {
    camera.stopCapture();
    camera.dispose();
  }

  public static void InitializePeerConncectionFactory(Context context) throws InterruptedException {
    PeerConnectionFactory.initialize(
        PeerConnectionFactory.InitializationOptions.builder(context).createInitializationOptions());
  }
}
