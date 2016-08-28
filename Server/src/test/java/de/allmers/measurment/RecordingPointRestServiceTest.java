package de.allmers.measurment;

import de.allmers.HumitempApplication;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.boot.test.SpringApplicationConfiguration;
import org.springframework.test.context.junit4.SpringJUnit4ClassRunner;
import org.springframework.test.context.web.WebAppConfiguration;

import static org.junit.Assert.*;

/**
 * Created by joern on 10.07.2016.
 */i
@RunWith(SpringJUnit4ClassRunner.class)
@SpringApplicationConfiguration(classes = HumitempApplication.class)
@WebAppConfiguration
public class RecordingPointRestServiceTest {

    private RecordingPointRepository recordingPointRepository;

    @Test
    public void souldGetIdOfRecordingPointWhenNotCreatedBefore(){
        recordingPointRepository.getRecordingPoint("Dachboden");
    }

}