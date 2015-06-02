Name:           ros-hydro-pr2-gripper-sensor
Version:        1.0.8
Release:        0%{?dist}
Summary:        ROS pr2_gripper_sensor package

Group:          Development/Libraries
License:        TODO
Source0:        %{name}-%{version}.tar.gz

BuildArch:      noarch

Requires:       ros-hydro-pr2-gripper-sensor-action
Requires:       ros-hydro-pr2-gripper-sensor-controller
Requires:       ros-hydro-pr2-gripper-sensor-msgs
BuildRequires:  ros-hydro-catkin

%description
The pr2_gripper_sensor package

%prep
%setup -q

%build
# In case we're installing to a non-standard location, look for a setup.sh
# in the install tree that was dropped by catkin, and source it.  It will
# set things like CMAKE_PREFIX_PATH, PKG_CONFIG_PATH, and PYTHONPATH.
if [ -f "/opt/ros/hydro/setup.sh" ]; then . "/opt/ros/hydro/setup.sh"; fi
mkdir -p obj-%{_target_platform} && cd obj-%{_target_platform}
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
cd obj-%{_target_platform}
make %{?_smp_mflags} install DESTDIR=%{buildroot}

%files
/opt/ros/hydro

%changelog
* Tue Jun 02 2015 Devon Ash <dash@clearpathrobotics.com> - 1.0.8-0
- Autogenerated by Bloom

* Thu Mar 26 2015 Devon Ash <dash@clearpathrobotics.com> - 1.0.7-0
- Autogenerated by Bloom

* Tue Mar 17 2015 Devon Ash <dash@clearpathrobotics.com> - 1.0.6-0
- Autogenerated by Bloom

* Mon Nov 03 2014 Devon Ash <dash@clearpathrobotics.com> - 1.0.4-0
- Autogenerated by Bloom

* Thu Sep 18 2014 Devon Ash <dash@clearpathrobotics.com> - 1.0.3-0
- Autogenerated by Bloom

* Wed Sep 10 2014 Devon Ash <dash@clearpathrobotics.com> - 1.0.2-0
- Autogenerated by Bloom

