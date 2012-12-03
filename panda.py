from math import pi, sin, cos
 
from direct.showbase.ShowBase import ShowBase
from direct.task import Task
from direct.actor.Actor import Actor
from pandac.PandaModules import *
from direct.task import Task
from direct.showbase.DirectObject import DirectObject
import sys
 
class MyApp(ShowBase):
    def __init__(self):
        ShowBase.__init__(self)
 
        # Load the environment model.
        self.environ = self.loader.loadModel("models/environment")
        # Reparent the model to render.
        self.environ.reparentTo(self.render)
        # Apply scale and position transforms on the model.
        self.environ.setScale(0.25, 0.25, 0.25)
        self.environ.setPos(-8, 42, 0)
 
        # Add the spinCameraTask procedure to the task manager.
        self.taskMgr.add(self.spinCameraTask, "SpinCameraTask")
        #self.taskMgr.add(self.myTask, "myTask")
 
        # Load and transform the panda actor.
        self.pandaActor = Actor("models/panda-model",
                                {"walk": "models/panda-walk4"})
        self.pandaActor.setScale(0.005, 0.005, 0.005)
        self.pandaActor.reparentTo(self.render)
        # Loop its animation.
        self.pandaActor.loop("walk", fromFrame=0, toFrame=0)
        # self.pandaActor.pprint()
        #print self.pandaActor.getCurrentFrame("walk", "modelRoot")
        # print self.pandaActor.getNumFrames("walk")
        #print self.pandaActor.getCurrentFrame("walk")
        # self.pandaActor.pose("walk", 0)
        #self.pandaActor.getAnimControl("walk").pose(0)
        # self.pandaActor.listJoints()

        self.count = 0
        self.frame = 0

    def get_geom(self, start):
        for c in start.getChildren():
            tmp = self.get_geom(c)
            if tmp is not None:
                return tmp
        for n in start.getNodes():
            if isinstance(n, GeomNode):
                tmp = self.geom_node(n)
                if tmp is not None:
                    return tmp
        return None

    def geom_node(self, n):
        for c in n.getGeoms():
            if isinstance(c, Geom):
                return c
        return None
 
    # Define a procedure to move the camera.
    def spinCameraTask(self, task):
        if self.count < 10:
            self.count += 1
        else:
            geom = self.get_geom(self.pandaActor.getGeomNode())
            print geom.getNestedVertices()
            print geom.getVertexData().getNumRows()
            print geom.getVertexData().animateVertices(True, Thread.getCurrentThread())
            sys.exit(0)
        
        # if self.count < 10:
        #     self.count += 1
        # else:
        #     joints = self.pandaActor.getJoints()
        #     for j in joints:
        #         print j.getName()
        #         local = Mat4()
        #         world = Mat4()
        #         j.getTransform(local)
        #         local.transposeInPlace()
        #         #print local
        #         j.getNetTransform(world)
        #         world.transposeInPlace()
        #         print world
        #         binding = j.get_default_value()
        #         binding = j.get_value()
        #         binding.transposeInPlace()
        #         #print binding
        #     sys.exit(0)

        # if self.count < 10:
        #     self.count += 1
        # else:
        #     joints = self.pandaActor.getJoints()
        #     local = Mat4()
        #     world = Mat4()
        #     for j in joints:
        #         print j.getName()
        #         j.getTransform(local)
        #         local.transposeInPlace()
        #         #print local
        #         j.getNetTransform(world)
        #         world.transposeInPlace()
        #         #print world
        #     self.frame += 1
        #     self.count = 0
        #     if (self.frame == 62):
        #         sys.exit(0)
        #     self.pandaActor.loop("walk", fromFrame=self.frame, toFrame=self.frame)

        angleDegrees = task.time * 6.0
        angleRadians = angleDegrees * (pi / 180.0)
        self.camera.setPos(20 * sin(angleRadians), -20.0 * cos(angleRadians), 3)
        self.camera.setHpr(angleDegrees, 0, 0)
        return Task.cont
 
app = MyApp()
app.run()
