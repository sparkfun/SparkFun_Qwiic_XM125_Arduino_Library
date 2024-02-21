#pragma once

#include "SparkFun_Toolkit.h"
#include "sfeXM125Regs.h"

class QwDevXM125
{
    public:
        /// @brief Initializer
        QwDevXM125();

        /// @brief This function begins the examples/communication.
        /// @return Error code (0 no error)
        bool begin(sfeTkII2C *theBus = nullptr);

        int32_t returnRegister(uint32_t &regVal);

        // --------------------- I2C Disance Detector Functions ---------------------

        /// @brief This function sets all the beginning values for a basic I2C
        ///  example to be run on the device for presence sensing. 
        /// @return Error code (0 no error)
        int32_t distanceBegin(uint32_t start = 1000, uint32_t stop = 5000);

        /// @brief This function returns the version number of the device 
        ///  structure: major.minor.patch
        /// @return Error code (0 no error)
        int32_t getDistanceDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch);

        /// @brief This function returns if there was an error from the 
        ///  protocol status register 
        /// @return Error code (0 no error)
        int32_t getDistanceDetectorError(uint32_t &error);

        /// @brief This function returns the error status according to the bit 
        ///  mask value for the distance devices errors and busy bit 
        /// @param status Error status of device (see function for exact error)
        /// @return Error code (0 no error)
        int32_t getDistanceDetectorErrorStatus(uint32_t &status);

        /// @brief This function returns the measure counter, the number of measurements
        ///   performed since restart. 
        /// @return Error code (0 no error)
        int32_t getDistanceMeasureCounter(uint32_t &counter);

        /// @brief This function returns the status of the device if there are any issues 
        ///  with any of the status's listed defined. 
        /// @return Error code (0 no error)
        int32_t getDistanceDetectorStatus(uint32_t &status);

        /// @brief This function returns the number of detected distances.
        /// @param distance Number of detected distances
        /// @return Error code (0 no error)
        int32_t getDistanceNumberDistances(uint32_t &distance);

        /// @brief This function returns the indication that there might be an object 
        ///   near the start point of the measured range.
        /// @param edge Flag to determine object in range 
        /// @return Error code (0 no error)
        int32_t getDistanceNearStartEdge(uint32_t &edge);

        /// @brief This function returns the indication of a sensor calibration needed.
        /// @param calibrate Flag to indicate calibration required 
        /// @return Error code (0 no error)
        int32_t getDistanceCalibrationNeeded(uint32_t &calibrate);

        /// @brief This function returns if the measure command failed. 
        /// @param error Flag to indicate measure command error 
        /// @return Error code (0 no error)
        int32_t getDistanceMeasureDistanceError(uint32_t &error);

        /// @brief This function returns the temperature in sensor during measurements
        ///   (in degree Celsius). Note that it has poor absolute accuracy and should 
        ///   only be used for relative temperature measurements. 
        /// @param temperature Relative tempertaure of device 
        /// @return Error code (0 no error)
        int32_t getDistanceTemperature(uint32_t &temperature);

        /// @brief This function returns the distance to peak 0
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak0Distance(uint32_t &peak);

        /// @brief This function returns the distance to peak 1
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak1Distance(uint32_t &peak);

        /// @brief This function returns the distance to peak 2
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak2Distance(uint32_t &peak);

        /// @brief This function returns the distance to peak 3
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak3Distance(uint32_t &peak);

        /// @brief This function returns the distance to peak 4
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak4Distance(uint32_t &peak);

        /// @brief This function returns the distance to peak 5
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak5Distance(uint32_t &peak);

        /// @brief This function returns the distance to peak 6
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak6Distance(uint32_t &peak);

        /// @brief This function returns the distance to peak 7
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak7Distance(uint32_t &peak);

        /// @brief This function returns the distance to peak 8
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak8Distance(uint32_t &peak);

        /// @brief This function returns the distance to peak 9
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak9Distance(uint32_t &peak);

        /// @brief This function returns the strength of peak 0
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak0Strength(uint32_t &peak);

        /// @brief This function returns the strength of peak 1
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak1Strength(uint32_t &peak);

        /// @brief This function returns the strength of peak 2
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak2Strength(uint32_t &peak);

        /// @brief This function returns the strength of peak 3
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak3Strength(uint32_t &peak);

        /// @brief This function returns the strength of peak 4
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak4Strength(uint32_t &peak);

        /// @brief This function returns the strength of peak 5
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak5Strength(uint32_t &peak);

        /// @brief This function returns the strength of peak 6
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak6Strength(uint32_t &peak);

        /// @brief This function returns the strength of peak 7
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak7Strength(uint32_t &peak);

        /// @brief This function returns the strength of peak 8
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak8Strength(uint32_t &peak);

        /// @brief This function returns the strength of peak 9
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak9Strength(uint32_t &peak);

        /// @brief This function returns the start of measured interval 
        ///  in millimeters. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 250
        /// @return Error code (0 no error)
        int32_t getDistanceStart(uint32_t &startVal);

        /// @brief This function sets the start of measured interval in 
        ///  millimeters. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 250
        /// @return Error code (0 no error)
        int32_t setDistanceStart(uint32_t start);

        /// @brief This function returns the end of measured interval 
        ///  in millimeters. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 3000
        /// @return Error code (0 no error)
        int32_t getDistanceEnd(uint32_t &end);

        /// @brief This function sets the end of measured interval 
        ///  in millimeters. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 3000
        /// @return Error code (0 no error)
        int32_t setDistanceEnd(uint32_t end);

        /// @brief This function returns the used to limit step length.
        ///  If set to 0 (default), the step length is calculated 
        ///  based on profile. 
        ///  Default Value: 0
        /// @return Error code (0 no error)
        int32_t getDistanceMaxStepLength(uint32_t &length);

        /// @brief This function sets the used to limit step length.
        ///  If set to 0 (default), the step length is calculated 
        ///  based on profile. 
        ///  Default Value: 0
        /// @return Error code (0 no error)
        int32_t setDistanceMaxStepLength(uint32_t length);

        /// @brief This function reads if the close range leakage 
        ///  cancellation logic is enabled. 
        ///  Default Value: true 
        /// @return Error code (0 no error)
        int32_t getDistanceCloseRangeLeakageCancellation(bool &range);

        /// @brief This function sets the close range leakage 
        ///  cancellation logic. 
        ///  Default Value: true 
        /// @return Error code (0 no error)
        int32_t setDistanceCloseRangeLeakageCancellation(bool range);

        /// @brief This function returns the high signal quality in a 
        ///  better SNR (because of higher HWAAS) and higher power consumption.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 15000
        /// @return Error code (0 no error)
        int32_t getDistanceSignalQuality(uint32_t &signal);

        /// @brief This function sets the high signal quality in a 
        ///  better SNR (because of higher HWAAS) and higher power consumption.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 15000
        /// @return Error code (0 no error)
        int32_t setDistanceSignalQuality(uint32_t signal);

        /// @brief This function returns the max profile of the device. 
        ///  Default value = PROFILE5
        /// @return Error code (0 no error) 
        int32_t getDistanceMaxProfile(uint32_t &profile);

        /// @brief This function sets the max profile of the device 
        ///  Default value = PROFILE5
        /// @return Error code (0 no error)
        int32_t setDistanceMaxProfile(uint32_t profile);

        /// @brief This function returns the threshold method 
        /// @return Error code (0 no error)
        int32_t getDistanceThresholdMethod(uint32_t &method);

        /// @brief This function sets the threshold method
        /// @param method Threshold method (enum)
        /// @return Error code (0 no error)
        int32_t setDistanceThresholdMethod(uint32_t method);

        /// @brief This function returns the peak sorting method
        /// @param peak Peak sorting method 
        /// @return Error code (0 no error)
        int32_t getDistancePeakSorting(uint32_t &peak);

        /// @brief This function sets the peak sorting method 
        /// @param peak Peak sorting method 
        /// @return Error code (0 no error) 
        int32_t setDistancePeakSorting(uint32_t peak);

        /// @brief This function returns the number frames to use for recorded threshold. 
        ///  Default Value: 100
        /// @param thresh Number of frames 
        /// @return Error code (0 no error) 
        int32_t getDistanceNumFramesRecordedThreshold(uint32_t &thresh);

        /// @brief This function sets the number frames to use for recorded threshold. 
        ///  Default Value: 100
        /// @param thresh Number of frames 
        /// @return Error code (0 no error) 
        int32_t setDistanceNumFramesRecordedThreshold(uint32_t thresh);

        /// @brief This function returns the fixed amplitude threshold value. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 100000
        /// @param thresh Fixed amplitude threshold value
        /// @return Error code (0 no error) 
        int32_t getDistanceFixedAmpThreshold(uint32_t &thresh);

        /// @brief This function sets the fixed amplitude threshold value. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 100000
        /// @param thresh Fixed amplitude threshold value
        /// @return Error code (0 no error) 
        int32_t setDistanceFixedAmpThreshold(uint32_t thresh);

        /// @brief This function returns the threshold sensitivity 
        ///  (0 <= sensitivity <= 1000)
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 500
        /// @param thresh& Threshold sensitivity  
        /// @return Error code (0 no error) 
        int32_t getDistanceThresholdSensitivity(uint32_t &thresh);

        /// @brief This function sets the threshold sensitivity 
        ///  (0 <= sensitivity <= 1000)
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 500
        /// @param thresh Threshold sensitivity  
        /// @return Error code (0 no error) 
        int32_t setDistanceThresholdSensitivity(uint32_t thresh);

        /// @brief This function returns the reflector shape
        ///   Default Value: GENERIC
        /// @param shape Generic or planar reflection 
        /// @return Error code (0 no error) 
        int32_t getDistanceReflectorShape(uint32_t &shape);

        /// @brief This function sets the reflector shape
        ///   Default Value: GENERIC
        /// @param shape Generic or planar reflection 
        /// @return Error code (0 no error) 
        int32_t setDistanceReflectorShape(uint32_t shape);

        /// @brief This function returns the fixed strength threshold value. 
        ///  Default Value: 0
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @param thresh fixed threshold strength value 
        /// @return Error code (0 no error) 
        int32_t getDistanceFixedStrengthThresholdValue(int32_t &thresh);

        /// @brief This function sets the fixed strength threshold value. 
        ///  Default Value: 0
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @param thresh fixed threshold strength value 
        /// @return Error code (0 no error) 
        int32_t setDistanceFixedStrengthThresholdValue(int32_t thresh);

        /// @brief This function returns the measure on wakeup status.
        ///   Default Value: false
        /// @param measure Measure on wakeup occurance 
        /// @return Error code (0 no error) 
        int32_t getDistanceMeasureOneWakeup(bool &measure);

        /// @brief This function sets the measure on wakeup status.
        ///   Default Value: false
        /// @param measure Measure on wakeup occurance 
        /// @return Error code (0 no error) 
        int32_t setDistanceMeasureOneWakeup(bool measure);

        /// @brief This function sets the specific execute command as defined in the 
        ///   datasheet on Page 25, Section 6.2.40 Command.
        /// @param apply Enable configuration and calibrate
        /// @return Error code (0 no error) 
        int32_t setDistanceCommand(uint32_t command);

        /// @brief Completes a busy wait loop while the device is uploading
        ///  information by waiting for the status
        /// @return Error code (0 no error) 
        int32_t distanceBusyWait();

        // Test function
        int32_t getDistanceReg(uint32_t &regVal);

        // --------------------- I2C Presence Detector Functions ---------------------

        /// @brief This function sets all the beginning values for a basic I2C
        ///  example to be run on the device for presence sensing. 
        /// @param start Value in mm to start, default 1000mm 
        /// @param stop  Value in mm to stop, default 5000mm
        /// @return Error code (0 no error)
        int32_t presenceDetectorStart(uint32_t start = 1000, uint32_t stop = 5000);

        /// @brief This function returns the RSS version number     
        /// @param version Version number
        /// @param patch Patch version number 
        /// @return Error code (0 no error)
        int32_t getPresenceDetectorVersion(uint32_t &major, uint32_t &minor, uint32_t &patch);

        /// @brief This function returns the protocol state erorr 
        /// @param state Error code for device 
        ///   0 = Protocol state error 
        ///   1 = Packet length error 
        ///   2 = Address error
        ///   3 = Write failed
        ///   4 = Write to read only 
        /// @return Error code (0 no error)  
        int32_t getPresenceDetectorError(uint32_t &error);

        /// @brief This function returns the measure counter; the number of 
        ///  measurements performed since restart.
        /// @param counter number of measurements
        /// @return Error code (0 no error)  
        int32_t getPresenceMeasureCounter(uint32_t &counter);

        /// @brief This function returns the detector status flags. See page 17, section 
        ///   6.2.4 Detector Status 
        /// @param status Status detector flag 
        /// @return Error code (0 no error)  
        int32_t getPresenceDetectorStatus(uint32_t &status);

        /// @brief This function returns the error status according to the bit 
        ///  mask value for the presence devices errors and busy bit 
        /// @param status Error status of device (see function for exact error)
        /// @return Error code (0 no error)
        int32_t getPresenceDetectorErrorStatus(uint32_t &status);

        /// @brief This function returns if there was presence detected
        /// @param detected Presence Detected 
        /// @return Error code (0 no error) 
        int32_t getPresenceDetectorPresenceDetected(uint32_t &detected);

        /// @brief This function returns if there was presence detected, sticky bit with
        ///  clear on read.
        /// @param sticky Sticky Presence Detected 
        /// @return Error code (0 no error) 
        int32_t getPresenceDetectorPresenceStickyDetected(uint32_t &sticky);

        /// @brief This function returns the indication that the presence detector
        ///  failed. 
        /// @param calibrate Indication of sensor calibration 
        /// @return Error code (0 no error) 
        int32_t getPresenceDetectorRegError(uint32_t &error);

        /// @brief This function returns the temperature in sensor during measurement (in
        ///   degree Celsius). Note that it has poor absolute accuracy and should only 
        ///   be used for relavtive temperature measurements. 
        /// @param temp Relative temperature
        /// @return Error code (0 no error) 
        int32_t getPresenceTemperature(uint32_t &temp);

        /// @brief This function returns the distance, in millimeters, for the detected
        ///  presence. 
        /// @param distance distance in mm
        /// @return Error code (0 no error) 
        int32_t getPresenceDistance(uint32_t &distance);

        /// @brief This function returns the measure of amount of fast motion detected. 
        /// @param intra Measure of fast motion 
        /// @return Error code (0 no error) 
        int32_t getPresenceIntraPresenceScore(uint32_t &intra);

        /// @brief This function returns the measure of amount of slow motion detected. 
        /// @param inter Measure of slow motion 
        /// @return Error code (0 no error) 
        int32_t getPresenceInterPresenceScore(uint32_t &inter);

        /// @brief This function returns the number of sweeps that will be 
        ///   captured in each frame (measurement).
        ///   Default Value: 16 seconds
        /// @param sweeps Number of sweeps
        /// @return Error code (0 no error) 
        int32_t getPresenceSweepsPerFrame(uint32_t &sweeps);

        /// @brief This function sets the number of sweeps that will be 
        ///   captured in each frame (measurement).
        ///   Default Value: 16 seconds
        /// @param sweeps Number of sweeps
        /// @return Error code (0 no error) 
        int32_t setPresenceSweepsPerFrame(uint32_t sweeps);

        /// @brief Returns the number of seconds the inter-frame presence score needs to decrease before
        ///  exponential scaling starts for faster decline. Should be between 0 and 30 where 0
        ///  means no timeout.
        ///   Default Value: 3 seconds
        /// @param time Number of seconds 
        /// @return Error code (0 no error) 
        int32_t getPresenceInterFramePresenceTimeout(uint32_t &time);

        /// @brief Sets the number of seconds the inter-frame presence score needs to decrease before
        ///  exponential scaling starts for faster decline. Should be between 0 and 30 where 0
        ///  means no timeout.
        ///   Default Value: 3 seconds
        /// @param time Number of seconds 
        /// @return Error code (0 no error) 
        int32_t setPresenceInterFramePresenceTimeout(uint32_t time);

        /// @brief This function returns the enable to increase detection of slow 
        ///  motions by utilizing the phase information in the Sparse IQ data
        /// @param en Enables increased detection 
        ///   Default Value: false
        /// @return Error code (0 no error) 
        int32_t getPresenceInterPhaseBoostEnabled(bool &en);

        /// @brief This function sets the enable to increase detection of slow 
        ///  motions by utilizing the phase information in the Sparse IQ data
        /// @param en Enables increased detection 
        ///   Default Value: false
        /// @return Error code (0 no error) 
        int32_t setPresenceInterPhaseBoostEnabled(bool en);

        /// @brief This function returns the bit to enable to detect faster 
        ///  movements inside frames. 
        ///   Default Value: true
        /// @param en Enables faster detection 
        /// @return Error code (0 no error)
        int32_t getPresenceIntraDetectionEnabled(bool &en);

        /// @brief This function returns the bit to enable to detect slower
        ///  movements between frames. 
        ///   Default Value: true
        /// @param en Enables faster detection 
        /// @return Error code (0 no error)
        int32_t setPresenceInterDetectionEnabled(bool en);

        /// @brief This function returns the presence detector frame rate. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 12000 mHz
        /// @param rate Frame rate, in mHz
        /// @return Error code (0 no error)
        int32_t getPresenceFrameRate(uint32_t &rate);

        /// @brief This function sets the presence detector frame rate. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 12000 mHz
        /// @param rate Frame rate, in mHz
        /// @return Error code (0 no error)
        int32_t setPresenceFrameRate(uint32_t rate);

        /// @brief This function returns the threshold for detecting faster 
        ///  movements inside frames. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 1300
        /// @param thresh threshold for faster movements 
        /// @return Error code (0 no error)
        int32_t getPresenceIntraDetectionThreshold(uint32_t &thresh);

        /// @brief This function sets the threshold for detecting faster 
        ///  movements inside frames. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 1300
        /// @param thresh threshold for faster movements 
        /// @return Error code (0 no error)
        int32_t setPresenceIntraDetectionThreshold(uint32_t thresh);

        /// @brief This function returns the threshold for detecting slower 
        ///  movements inside frames. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 1000
        /// @param thresh threshold for faster movements 
        /// @return Error code (0 no error)
        int32_t getPresenceInterDetectionThreshold(uint32_t &thresh);

        /// @brief This function sets the threshold for detecting slower 
        ///  movements inside frames. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 1000
        /// @param thresh threshold for faster movements 
        /// @return Error code (0 no error)
        int32_t setPresenceInterDetectionThreshold(uint32_t thresh);

        /// @brief This function returns the time constant of the low pass 
        ///  filter for the inter-frame deviation between fast and slow.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 500 ms
        /// @param time time in ms
        /// @return Error code (0 no error)
        int32_t getPresenceInterFrameDeviationTime(uint32_t &time);

        /// @brief This function sets the time constant of the low pass 
        ///  filter for the inter-frame deviation between fast and slow.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 500 ms
        /// @param time time in ms
        /// @return Error code (0 no error)
        int32_t setPresenceInterFrameDeviationTime(uint32_t time);

        /// @brief This function returns the cutoff frequency of the low pass 
        ///  filter for the fast filtered absolute sweep mean.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 6000 mHz
        /// @param cut cutoff frequency in mHz
        /// @return Error code (0 no error)
        int32_t getPresenceInterFrameFastCutoff(uint32_t &cut);

        /// @brief This function sets the cutoff frequency of the low pass 
        ///  filter for the fast filtered absolute sweep mean.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 6000 mHz
        /// @param cut cutoff frequency in mHz
        /// @return Error code (0 no error)
        int32_t setPresenceInterFrameFastCutoff(uint32_t cut);

        /// @brief This function returns the cutoff frequency of the low pass 
        ///  filter for the slow filtered absolute sweep mean.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 200 mHz
        /// @param cut cutoff frequency in mHz
        /// @return Error code (0 no error)
        int32_t getPresenceInterFrameSlowCutoff(uint32_t &cut);

        /// @brief This function sets the cutoff frequency of the low pass 
        ///  filter for the slow filtered absolute sweep mean.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 200 mHz
        /// @param cut cutoff frequency in mHz
        /// @return Error code (0 no error)
        int32_t setPresenceInterFrameSlowCutoff(uint32_t cut);

        /// @brief This functions returns time constant for the depthwise 
        ///  filtering in the intra-frame part
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 150ms
        /// @param time time const in ms
        /// @return Error code (0 no error)
        int32_t getPresenceIntraFrameTimeConst(uint32_t &time);

        /// @brief This functions sets time constant for the depthwise 
        ///  filtering in the intra-frame part
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 150ms
        /// @param time time const in ms
        /// @return Error code (0 no error)
        int32_t setPresenceIntraFrameTimeConst(uint32_t time);

        /// @brief This function returns time constant for the output in 
        ///  the intra-frame part
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 300ms
        /// @param time time const in ms
        /// @return Error code (0 no error)
        int32_t getPresenceIntraOutputTimeConst(uint32_t &time);

        /// @brief This function sets the time constant for the output in 
        ///  the intra-frame part
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 300ms
        /// @param time time const in ms
        /// @return Error code (0 no error)
        int32_t setPresenceIntraOutputTimeConst(uint32_t time);

        /// @brief This function returns time constant for the output in 
        ///  the inter-frame part
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 2000ms
        /// @param time time const in ms
        /// @return Error code (0 no error)
        int32_t getPresenceInterOutputTimeConst(uint32_t &time);

        /// @brief This function sets the time constant for the output in 
        ///  the inter-frame part
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 2000ms
        /// @param time time const in ms
        /// @return Error code (0 no error)
        int32_t setPresenceInterOutputTimeConst(uint32_t time);

        /// @brief This function returns if enable/disable for the automatic  
        ///  selection of profile based on start point of measurement.
        ///   Default Value: true
        /// @param en enable/disable selection
        /// @return Error code (0 no error)
        int32_t getPresenceAutoProfileEn(bool &en);

        /// @brief This function enables/disables the automatic selection 
        ///  of profile based on start point of measurement.
        ///   Default Value: true
        /// @param en enable/disable selection
        /// @return Error code (0 no error)
        int32_t setPresenceAutoProfileEn(bool en);

        /// @brief This function returns the Enable/Disable 
        ///  automatic selection of step length based on the profile.
        /// @param en Enable/disable selection
        /// @return Error code (0 no error)
        int32_t getPresenceAutoStepLengthEn(bool &en);

        /// @brief This function Enables/Disables automatic selection
        ///  of step length based on the profile.
        /// @param en Enable/disable selection
        /// @return Error code (0 no error)
        int32_t setPresenceAutoStepLengthEn(bool en);

        /// @brief This function returns the profile in use. The profile will only 
        ///  be used if profile auto selection was disabled. 
        /// @param prof Profile to use (enum)
        /// @return Error code (0 no error)
        int32_t getPresenceManualProfile(uint32_t &prof);

        /// @brief This function sets the profile to use. The profile will only 
        ///  be used if profile auto selection was disabled. 
        /// @param prof Profile to use (enum)
        /// @return Error code (0 no error)
        int32_t setPresenceManualProfile(uint32_t prof);

        /// @brief THis function returns the number of steps between each data point. 
        ///  NOTE: The manual step length will only be used if step length auto selection 
        ///  was disabled
        ///   Default Value: 72
        /// @param length number of steps
        /// @return Error code (0 no error)
        int32_t getPresenceManualStepLength(uint32_t &length);

        /// @brief THis function returns the number of steps between each data point. 
        ///  NOTE: The manual step length will only be used if step length auto selection 
        ///  was disabled
        ///   Default Value: 72
        /// @param length number of steps 
        /// @return Error code (0 no error)
        int32_t setPresenceManualStepLength(uint32_t length);

        /// @brief This function returns the start of measured interval in mm. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 300mm
        /// @param start measured start interval in mm
        /// @return Error code (0 no error) 
        int32_t getPresenceStart(uint32_t &start);

        /// @brief This function sets the start of measured interval in mm. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 300mm
        /// @param start measured start interval in mm
        /// @return Error code (0 no error) 
        int32_t setPresenceStart(uint32_t start);

        /// @brief This function returns the end point of measurement 
        ///  interval in millimeters.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 2500
        /// @param end End point of measurement in mm 
        /// @return Error code (0 no error) 
        int32_t getPresenceEnd(uint32_t &end);

        /// @brief This function returns the end point of measurement 
        ///  interval in millimeters.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 2500
        /// @param end End point of measurement in mm 
        /// @return Error code (0 no error) 
        int32_t setPresenceEnd(uint32_t end);

        /// @brief This function returns if the device is enabled/disabled 
        ///  for the reset of the presence filters during start/restart.
        ///   Default Value: true
        /// @param reset enabled/disabled 
        /// @return Error code (0 no error) 
        int32_t getPresenceResetFilters(bool &reset);

        /// @brief This function sets if the device is enabled/disabled 
        ///  for the reset of the presence filters during start/restart .
        ///   Default Value: true
        /// @param reset enabled/disabled 
        /// @return Error code (0 no error) 
        int32_t setPresenceResetFilters(bool reset);

        /// @brief This function returns the hardware accelerated average
        ///  sampled (HWAAS).
        ///   Default Value: 32
        /// @param avg average samples 
        /// @return Error code (0 no error) 
        int32_t getPresenceHWAAS(uint32_t &avg);

        /// @brief This function sets the hardware accelerated average
        ///  sampled (HWAAS).
        ///   Default Value: 32
        /// @param avg average samples 
        /// @return Error code (0 no error) 
        int32_t setPresenceHWAAS(uint32_t avg);

        /// @brief This function returns the output presence detection on 
        ///  generic GPIO.
        /// @param detected Output presence detected 
        /// @return Error code (0 no error) 
        int32_t getPresenceDetectionOnGPIO(uint32_t &detected);

        /// @brief This function returns the output presence detection on 
        ///  generic GPIO. 
        /// @param detected Output presence detected 
        /// @return Error code (0 no error) 
        int32_t setPresenceDetectionOnGPIO(uint32_t detected); 

        /// @brief This function sets the execute command
        /// @param cmd command to send to device 
        /// @return Error code (0 no error) 
        int32_t setPresenceCommand(uint32_t cmd);

        /// @brief This function resets the presence detector settings 
        ///  of the device
        /// @return Error code (0 no error)
        int32_t presenceReset();

        /// @brief This function returns the busy bit of the presence status register
        /// @param busy Device busy or not 
        /// @return Error code (0 no error)
        int32_t getPresenceBusy(uint32_t &busy);

        /// @brief Completes a busy wait loop while the device is uploading
        ///  information by waiting for the status. Checks the error status register
        ///  to wait until errors are completed/gone
        /// @return Error code (0 no error) 
        int32_t presenceBusyWait();

        int32_t getPresenceRegisterVal(uint32_t &regVal);

        int32_t flipBytes(uint32_t &data);
        int32_t flipBytesInt(int32_t &data);


protected: 
        sfeTkII2C *_theBus;

};
