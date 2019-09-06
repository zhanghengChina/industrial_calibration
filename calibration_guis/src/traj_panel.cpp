
#include <stdio.h>

#include <QColor>
#include <QSlider>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QPainter>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <geometry_msgs/Twist.h>
#include <std_srvs/Trigger.h>
#include <calibration_guis/traj_panel.h>
#include <pluginlib/class_list_macros.h>



namespace calibration_guis
{


TrajectoryPanel::TrajectoryPanel( QWidget* parent )
  : rviz::Panel( parent )
{
  QHBoxLayout* topic_layout = new QHBoxLayout;
  topic_layout->addWidget( new QLabel( "Output Topic:" ));
  output_topic_editor_ = new QLineEdit;
  topic_layout->addWidget( output_topic_editor_ );


  // Lay out the topic field above the control widget.
  QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout( topic_layout );

    QPushButton* call_service_btn_1 = new QPushButton( "Capture" , parent );
    QPushButton* call_service_btn_2 = new QPushButton( "Execute" , parent );
    QPushButton* call_service_btn_3 = new QPushButton( "ExecuteWCall" , parent );
    QPushButton* call_service_btn_4 = new QPushButton( "MoveEnd" , parent );
    QPushButton* call_service_btn_5 = new QPushButton( "MoveNext" , parent );
    QPushButton* call_service_btn_6 = new QPushButton( "MovePrev" , parent );
    QPushButton* call_service_btn_7 = new QPushButton( "MoveStart" , parent );

    QGridLayout* controls_layout = new QGridLayout();
    controls_layout->addWidget( call_service_btn_1, 0, 0 );
    controls_layout->addWidget( call_service_btn_2, 1, 0 );
    controls_layout->addWidget( call_service_btn_3, 1, 1 );
    controls_layout->addWidget( call_service_btn_4, 3, 1 );
    controls_layout->addWidget( call_service_btn_5, 2, 0 );
    controls_layout->addWidget( call_service_btn_6, 2, 1 );
    controls_layout->addWidget( call_service_btn_7, 3, 0 );

    setLayout( controls_layout );


    // Next we make signal/slot connections.
    connect( call_service_btn_1, SIGNAL( clicked( bool )), this, SLOT( setbutton1Clicked()));
    connect( call_service_btn_2, SIGNAL( clicked( bool )), this, SLOT( setbutton2Clicked()));
    connect( call_service_btn_3, SIGNAL( clicked( bool )), this, SLOT( setbutton3Clicked()));
    connect( call_service_btn_4, SIGNAL( clicked( bool )), this, SLOT( setbutton4Clicked()));
    connect( call_service_btn_5, SIGNAL( clicked( bool )), this, SLOT( setbutton5Clicked()));
    connect( call_service_btn_6, SIGNAL( clicked( bool )), this, SLOT( setbutton6Clicked()));
    connect( call_service_btn_7, SIGNAL( clicked( bool )), this, SLOT( setbutton7Clicked()));

    connect( output_topic_editor_, SIGNAL( editingFinished() ), this, SLOT( updateTopic() ));


    ros::NodeHandle n("~");

    captureClient_      = n.serviceClient<std_srvs::Trigger>("/joint_traj/JTrajCapture");
    executeClient_      = n.serviceClient<std_srvs::Trigger>("/joint_traj/JTrajExecute");
    executeWCallClient_ = n.serviceClient<std_srvs::Trigger>("/joint_traj/JTrajExecuteWCall");
    moveEndClient_      = n.serviceClient<std_srvs::Trigger>("/joint_traj/JTrajMoveEnd");
    moveNextClient_     = n.serviceClient<std_srvs::Trigger>("/joint_traj/JTrajMoveNext");
    movePrevClient_     = n.serviceClient<std_srvs::Trigger>("/joint_traj/JTrajMovePrev");
    moveStartClient_    = n.serviceClient<std_srvs::Trigger>("/joint_traj/JTrajMoveStart");

  }


  // Read the topic name from the QLineEdit and call setTopic() with the
  // results.  This is connected to QLineEdit::editingFinished() which
  // fires when the user presses Enter or Tab or otherwise moves focus
  // away.
  void TrajectoryPanel::updateTopic()
  {
    setTopic( output_topic_editor_->text() );
  }

  // Set the topic name we are publishing to.
  void TrajectoryPanel::setTopic( const QString& new_topic )
  {
    // Only take action if the name has changed.
    if( new_topic != output_topic_ )
    {
      output_topic_ = new_topic;
      // If the topic is the empty string, don't publish anything.
      if( output_topic_ == "" )
      {
        velocity_publisher_.shutdown();
      }
      else
      {
        // The old ``velocity_publisher_`` is destroyed by this assignment,
        // and thus the old topic advertisement is removed.  The call to
        // nh_advertise() says we want to publish data on the new topic
        // name.
        velocity_publisher_ = nh_.advertise<geometry_msgs::Twist>( output_topic_.toStdString(), 1 );
      }
      // rviz::Panel defines the configChanged() signal.  Emitting it
      // tells RViz that something in this panel has changed that will
      // affect a saved config file.  Ultimately this signal can cause
      // QWidget::setWindowModified(true) to be called on the top-level
      // rviz::VisualizationFrame, which causes a little asterisk ("*")
      // to show in the window's title bar indicating unsaved changes.
      Q_EMIT configChanged();
    }

  }

  // Save all configuration data from this panel to the given
  // Config object.  It is important here that you call save()
  // on the parent class so the class id and panel name get saved.
  void TrajectoryPanel::save( rviz::Config config ) const
  {
    rviz::Panel::save( config );
    config.mapSetValue( "Topic", output_topic_ );
  }

  // Load all configuration data for this panel from the given Config object.
  void TrajectoryPanel::load( const rviz::Config& config )
  {
    rviz::Panel::load( config );
    QString topic;
    if( config.mapGetString( "Topic", &topic ))
    {
      output_topic_editor_->setText( topic );
      updateTopic();
    }
  }
  void TrajectoryPanel::setbutton1Clicked ()
  {
    std_srvs::Trigger srv;
    if (captureClient_.call(srv))
    {
      ROS_INFO("Output %s",srv.response.message.c_str());
    }
    else
    {
      ROS_ERROR("Failed to call capture Trigger");
    }
  }

  void TrajectoryPanel::setbutton2Clicked ()
  {
    std_srvs::Trigger srv;
    if (executeClient_.call(srv))
    {
      ROS_INFO("Output %s",srv.response.message.c_str());
    }
    else
    {
      ROS_ERROR("Failed to call execute Trigger");
    }
  }

  void TrajectoryPanel::setbutton3Clicked ()
  {
    std_srvs::Trigger srv;
    if (executeWCallClient_.call(srv))
    {
      ROS_INFO("Output %s",srv.response.message.c_str());
    }
    else
    {
      ROS_ERROR("Failed to call execute with call Trigger");
    }
  }

  void TrajectoryPanel::setbutton4Clicked ()
  {
    std_srvs::Trigger srv;
    if (moveEndClient_.call(srv))
    {
      ROS_INFO("Output %s",srv.response.message.c_str());
    }
    else
    {
      ROS_ERROR("Failed to call Move End Trigger");
    }
  }

  void TrajectoryPanel::setbutton5Clicked ()
  {
    std_srvs::Trigger srv;
    if (moveNextClient_.call(srv))
    {
      ROS_INFO("Output %s",srv.response.message.c_str());
    }
    else
    {
      ROS_ERROR("Failed to call Move Next Trigger");
    }
  }

  void TrajectoryPanel::setbutton6Clicked ()
  {
    std_srvs::Trigger srv;
    if (movePrevClient_.call(srv))
    {
      ROS_INFO("Output %s",srv.response.message.c_str());
    }
    else
    {
      ROS_ERROR("Failed to call Move Previous Trigger");
    }
  }

  void TrajectoryPanel::setbutton7Clicked ()
  {
    std_srvs::Trigger srv;
    if (moveStartClient_.call(srv))
    {
      ROS_INFO("Output %s",srv.response.message.c_str());
    }
    else
    {
      ROS_ERROR("Failed to call Move Start Trigger");
    }
  }


} // end namespace calibration_guis


PLUGINLIB_EXPORT_CLASS(calibration_guis::TrajectoryPanel,rviz::Panel)
