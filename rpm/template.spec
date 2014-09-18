Name:           ros-hydro-pr2-gripper-sensor-action
Version:        1.0.3
Release:        0%{?dist}
Summary:        ROS pr2_gripper_sensor_action package

Group:          Development/Libraries
License:        BSD
URL:            None
Source0:        %{name}-%{version}.tar.gz

Requires:       ros-hydro-actionlib
Requires:       ros-hydro-actionlib-msgs
Requires:       ros-hydro-message-runtime
Requires:       ros-hydro-pr2-controllers-msgs
Requires:       ros-hydro-pr2-gripper-sensor-controller
Requires:       ros-hydro-pr2-gripper-sensor-msgs
Requires:       ros-hydro-pr2-machine
Requires:       ros-hydro-pr2-mechanism-controllers
Requires:       ros-hydro-pr2-mechanism-model
Requires:       ros-hydro-robot-mechanism-controllers
Requires:       ros-hydro-roscpp
BuildRequires:  ros-hydro-actionlib
BuildRequires:  ros-hydro-actionlib-msgs
BuildRequires:  ros-hydro-catkin
BuildRequires:  ros-hydro-message-generation
BuildRequires:  ros-hydro-pr2-controllers-msgs
BuildRequires:  ros-hydro-pr2-gripper-sensor-controller
BuildRequires:  ros-hydro-pr2-gripper-sensor-msgs
BuildRequires:  ros-hydro-pr2-machine
BuildRequires:  ros-hydro-pr2-mechanism-controllers
BuildRequires:  ros-hydro-pr2-mechanism-model
BuildRequires:  ros-hydro-robot-mechanism-controllers
BuildRequires:  ros-hydro-roscpp

%description
The pr2_gripper_sensor_action package provides an action interface for talking
to the pr2_gripper_sensor_controller real-time controller. It provides several
different actions for getting high-level sensor information from the PR2 palm-
mounted accelerometers, fingertip pressure arrays, and gripper motor/encoder, as
well as several sensor-based gripper control actions that respond with low-
latency in real-time.

%prep
%setup -q

%build
# In case we're installing to a non-standard location, look for a setup.sh
# in the install tree that was dropped by catkin, and source it.  It will
# set things like CMAKE_PREFIX_PATH, PKG_CONFIG_PATH, and PYTHONPATH.
if [ -f "/opt/ros/hydro/setup.sh" ]; then . "/opt/ros/hydro/setup.sh"; fi
mkdir -p build && cd build
%cmake .. \
        -UINCLUDE_INSTALL_DIR \
        -ULIB_INSTALL_DIR \
        -USYSCONF_INSTALL_DIR \
        -USHARE_INSTALL_PREFIX \
        -ULIB_SUFFIX \
        -DCMAKE_INSTALL_PREFIX="/opt/ros/hydro" \
        -DCMAKE_PREFIX_PATH="/opt/ros/hydro" \
        -DSETUPTOOLS_DEB_LAYOUT=OFF \
        -DCATKIN_BUILD_BINARY_PACKAGE="1" \

make %{?_smp_mflags}

%install
# In case we're installing to a non-standard location, look for a setup.sh
# in the install tree that was dropped by catkin, and source it.  It will
# set things like CMAKE_PREFIX_PATH, PKG_CONFIG_PATH, and PYTHONPATH.
if [ -f "/opt/ros/hydro/setup.sh" ]; then . "/opt/ros/hydro/setup.sh"; fi
cd build
make %{?_smp_mflags} install DESTDIR=%{buildroot}

%files
/opt/ros/hydro

%changelog
* Thu Sep 18 2014 Devon Ash <dash@clearpathrobotics.com> - 1.0.3-0
- Autogenerated by Bloom

* Wed Sep 10 2014 Devon Ash <dash@clearpathrobotics.com> - 1.0.2-0
- Autogenerated by Bloom

