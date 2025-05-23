// -*- coding: utf-8 -*-
// Copyright (C) 2006-2013 Rosen Diankov <rosen.diankov@gmail.com>
//
// This file is part of OpenRAVE.
// OpenRAVE is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
/** \file planner.h
    \brief Planning related defintions.

    Automatically included with \ref openrave.h
 */
#ifndef OPENRAVE_PLANNER_H
#define OPENRAVE_PLANNER_H

namespace OpenRAVE {

/// \brief Controls what information gets validated when calling the constraints functions in planner parameters
///
/// By default, the lower 16 bits are set while the upper 16bits are zero.
enum ConstraintFilterOptions
{
    CFO_CheckEnvCollisions=1, ///< will check environment collisions with the robot (checked by default)
    CFO_CheckSelfCollisions=2, ///< will check the self-collision of the robot (checked by default).
    CFO_CheckTimeBasedConstraints=4, ///< checks any constraints that are depending on velocity. For example workspace accel constraints or dynamics.
    CFO_BreakOnFirstValidation=0x8, ///< if set, will break as soon as any constraint is violated (even if more than one are violated, the return code will only show one constraint breaking)
    CFO_CheckUserConstraints=0x00008000, ///< user-defined functions which don't fit any of the above descriptions
    CFO_CheckWithPerturbation=0x00010000, ///< when checking collisions, perturbs all the joint values a little and checks again. This forces the line to be away from grazing collisions.
    CFO_FillCheckedConfiguration=0x00020000, ///< if set, will fill \ref ConstraintFilterReturn::_configurations and \ref ConstraintFilterReturn::_configurationtimes
    CFO_FillCollisionReport=0x00040000, ///< if set, will fill \ref ConstraintFilterReturn::_report if in environment or self-collision
    CFO_FromPathSampling=0x00080000, ///< if set, will use \ref NSO_FromPathSampling for the _neighstatefn
    CFO_FromPathShortcutting=0x00100000, ///< if set, will use \ref NSO_FromPathShortcutting for the _neighstatefn
    CFO_FromTrajectorySmoother=0x00200000, ///< if set, will use \ref NSO_FromTrajectorySmoother for the _neighstatefn
    CFO_FinalValuesNotReached=0x40000000, ///< if set, then the final values of the interpolation have not been reached, although a close interpolation has been computed. This happens when manipulator constraints are used.
    CFO_StateSettingError=0x80000000, ///< error when the state setting function (or neighbor function) breaks
    CFO_RecommendedOptions = 0x0000ffff, ///< recommended options that all plugins should use by default
};

/// \brief the status of the PlanPath method. Used when PlanPath can be called multiple times to resume planning.
enum PlannerStatusCode
{
    PS_Failed = 0, ///< planner failed
    PS_HasSolution = 1, ///< planner succeeded
    PS_Interrupted = 2, ///< planning was interrupted, but can be resumed by calling PlanPath again
    PS_InterruptedWithSolution = 3, ///< planning was interrupted, but a valid path/solution was returned. Can call PlanPath again to refine results
    PS_FailedDueToEnvCollision = 0x00010000, ///< planner failed due to env collision constraints
    PS_FailedDueToSelfCollision = 0x00020000, ///< planner failed due to self collision constraints
    PS_FailedDueToCollision = (PS_FailedDueToEnvCollision|PS_FailedDueToSelfCollision), ///< planner failed due to any collision constraints
    PS_FailedDueToInitial = 0x00040000, ///< failed due to initial configurations
    PS_FailedDueToGoal = 0x00080000, ///< failed due to goal configurations
    PS_FailedDueToKinematics = 0x00100000, ///< failed due to kinematics constraints
    PS_FailedDueToIK = 0x00200000, ///< failed due to inverse kinematics (could be due to collisions or velocity constraints, but don't know)
    PS_FailedDueToVelocityConstraints = 0x00400000, ///< failed due to velocity constraints
    PS_FailedDueToCustomFilter = 0x00800000, ///< failed due to custom filter
    PS_FailedDueToTimeBasedConstraints = 0x01000000, ///< failed due to time based constraints such as torque
};

enum PlanningOptions
{
    PO_NoStatusDetail = 1, ///< if set, then do not output any PlannerStatus details, just the finish code. This allows system to be faster.
    PO_AddCollisionStatistics = 2 /// If set, Fill in collision to aid in planning failure debugging. This will slow down the system.
};

/// \brief action to send to the planner while it is planning. This is usually done by the user-specified planner callback function
enum PlannerAction
{
    PA_None=0, ///< no action
    PA_Interrupt=1, ///< interrupt the planner and return to user
    PA_ReturnWithAnySolution=2, ///< return quickly with any path
};

/// \brief options to supply to the _neighstatefn depending on how the neighbor should be computed.
///
/// The neighbor function takes a current position q and delta movement qdelta
enum NeighborStateOptions
{
    NSO_GoalToInitial=1, ///< if set, then q is coming from a goal state, else it is coming from an initial state.
    NSO_OnlyHardConstraints=2, ///< if set, then the new neighbor should be as close as possible to q+qdelta. This is used in smoothers when the path is already determined and user just wants to verify that hard constraints are met only; do not modify q+qdelta unless hard constraints fail.
    NSO_FromPathSampling=4, ///< if set, then the new neighbor needs to keep tighter constraint to give a room for the later processing. and if set, then it can prioritize the constraints and only use q+qdelta as a hint.
    NSO_FromPathShortcutting=8, ///< if set, then the new neighbor should be as close as possible to q+qdelta while the new neighbor needs to keep tighter constraint to give a room for the later processing. This is used from path shortcutter when the path is already determined and user just wants to verify that tighter constraints are met only; do not modify q+qdelta unless tighter constraints fail.
    NSO_FromTrajectorySmoother=16, ///< if set, then the new neighbor should be as close as possible to q+qdelta while the new neighbor needs to keep the constraint for smoother. When the path is already determined and user just wants to verify that tighter constraints are met only; do not modify q+qdelta unless tighter constraints fail.
};

/// \brief the status of _neighstatefn method. The first bit indicates if _neighstatefn call is successful. The second bit indicates, in case the call is successful, if the returned state is obtained from linear interpolation.
enum NeighborStateStatus
{
    NSS_Failed = 0, ///< _neighstatefn failed. The state Filter(q + qdelta) cannot be computed.
    NSS_Reached = 1, ///< _neighstatefn successful and the returned state is q + qdelta, i.e. linearly interpolated from q.
    NSS_SuccessfulWithDeviation = 3, ///< _neighstatefn successful but due to constraints, the returned state differs from q + qdelta.
};

/// \brief Return values for the constraint validation function.
class OPENRAVE_API ConstraintFilterReturn
{
public:
    ConstraintFilterReturn() : _fTimeWhenInvalid(0), _returncode(0), _bHasRampDeviatedFromInterpolation(false), _fTimeBasedSurpassMult(1.0) {
    }
    /// \brief clears the data
    inline void Clear() {
        _configurations.resize(0);
        _configurationtimes.resize(0);
        _invalidvalues.resize(0);
        _invalidvelocities.resize(0);
        _invalidaccelerations.resize(0);
        _returncode = 0;
        _fTimeWhenInvalid = 0;
        _bHasRampDeviatedFromInterpolation = false;
        _report.Reset();
        _fTimeBasedSurpassMult = 1.0;
    }

    std::vector<dReal> _configurations; ///< N*dof vector of the configurations used to check constraints. If the constraints were invalid, they stop at the first invalid constraint
    std::vector<dReal> _configurationtimes; ///< N vector of the times where each configuration was sampled at. If timeelapsed is set in the check path constraints function, then this is scaled by this time. Otherwise it is a value in [0,1] that describes the interpolation coefficient: 0 is the initial configuration, 1 is the final configuration.

    std::vector<dReal> _invalidvalues, _invalidvelocities, _invalidaccelerations; ///< if the constraint returned with an error, contains invalid configuration where it failed
    dReal _fTimeWhenInvalid; ///< if the constraint has an elapsed time, will contain the time when invalidated
    int _returncode; ///< if == 0, the constraint is good. If != 0 means constraint was violated and bitmasks in ConstraintFilterOptions can be used to find what constraint was violated.
    CollisionReport _report; ///< if in collision (_returncode&(CFO_CheckEnvCollisions|CFO_CheckSelfCollisions)), then stores the collision report
    bool _bHasRampDeviatedFromInterpolation; ///< if true, then it means that the checked ramp that passed is different from the interpolation expected on the start and end points, and the new points are filled in _configurations
    dReal _fTimeBasedSurpassMult; ///< if option contains CFO_CheckTimeBasedConstraints, then the multiplier is set to (some factor)*|max|/|actual max|. this is used for the reduction factor in velocity unit. if several constraints should be taken into account, take min.
};

typedef boost::shared_ptr<ConstraintFilterReturn> ConstraintFilterReturnPtr;

/** \brief Describes a common and serializable interface for planning parameters.

    The class is serializable to XML, so can be loaded from file or passed around the network.
    If extra parameters need to be specified, derive from this class and
    - add the extra tags to PlannerParameters::_vXMLParameters
    - override PlannerParameters::startElement and PlannerParameters::endElement for processing
    - possibly override the PlannerParameters::characters

    Also allows the parameters and descriptions to be serialized to reStructuredText for documentation purposes.
 */
class OPENRAVE_API PlannerParameters : public BaseXMLReader, public Readable
{
public:
    typedef std::list< std::vector<dReal> > ConfigurationList;
    typedef boost::shared_ptr< ConfigurationList > ConfigurationListPtr;

    PlannerParameters();
    virtual ~PlannerParameters();

    /// \brief saves and restores the state using PlannerParameters::_setstatevaluesfn and PlannerParameters::_getstatefn
    class OPENRAVE_API StateSaver
    {
public:
        StateSaver(boost::shared_ptr<PlannerParameters> params);
        virtual ~StateSaver();
        inline boost::shared_ptr<PlannerParameters> GetParameters() const {
            return _params;
        }
        virtual void Restore();
protected:
        boost::shared_ptr<PlannerParameters> _params;
        std::vector<dReal> _values;
private:
        virtual void _Restore();
    };

    typedef boost::shared_ptr<StateSaver> StateSaverPtr;

    /** \brief Attemps to copy data from one set of parameters to another in the safest manner.

        First serializes the data of the right hand into a string, then initializes the current parameters via >>
        pointers to functions are copied directly
     */
    virtual PlannerParameters& operator=(const PlannerParameters& r);
    virtual void copy(boost::shared_ptr<PlannerParameters const> r);

    virtual bool operator==(const Readable& other) const override {
        if (GetXMLId() != other.GetXMLId()) {
            return false;
        }
        const PlannerParameters* pOther = dynamic_cast<const PlannerParameters*>(&other);
        if (!pOther) {
            return false;
        }
        return this == pOther; // pointer compare, becase we cannot compare boost::function in this
    }

    virtual ReadablePtr CloneSelf() const override {
        boost::shared_ptr<PlannerParameters> pNew(new PlannerParameters());
        *pNew = *this;
        return pNew;
    }

    /// \brief sets up the planner parameters to use the active joints of the robot
    virtual void SetRobotActiveJoints(RobotBasePtr& probot);

    /// \brief sets up the planner to use specific DOF indices of the robot.
    virtual void SetRobotDOFIndices(RobotBasePtr& probot, const std::vector<int>& dofindices);

    /** \brief sets up the planner parameters to use the configuration specification space

        The configuraiton groups should point to controllable target objects. By default, this includes:
        - joint_values
        - joint_velocities
        - affine_transform
        - affine_velocities
        - grab
        The following internal parameters will be set:
        - _diffstatefn
        - _distmetricfn - weights used for distance metric are retrieved at this time and stored
        - _samplefn
        - _sampleneighfn
        - _setstatevaluesfn
        - _getstatefn
        - _neighstatefn
        - _checkpathconstraintsfn
        - _vConfigLowerLimit
        - _vConfigUpperLimit
        - _vConfigVelocityLimit
        - _vConfigAccelerationLimit
        - _vConfigJerkLimit
        - _vConfigResolution
        - vinitialconfig
        - _vInitialConfigVelocities - the initial velocities (at vinitialconfig) of the robot when starting to plan
        - _vGoalConfigVelocities - the goal velocities (at vinitialconfig) of the robot when finishing the plan
        - _configurationspecification
        \throw openrave_exception If the configuration specification is invalid or points to targets that are not present in the environment.
     */
    virtual void SetConfigurationSpecification(EnvironmentBasePtr env, const ConfigurationSpecification& spec);

    /// \brief veriries that the configuration space and all parameters are consistent
    ///
    /// Assumes at minimum that  _setstatevaluesfn and _getstatefn are set. Correct environment should be
    /// locked when this function is called since _getstatefn will be called.
    /// \throw openrave_exception If not consistent, will throw an exception
    virtual void Validate() const;

    /// \brief the configuration specification in which the planner works in. This specification is passed to the trajecotry creation modules.
    ConfigurationSpecification _configurationspecification;

    /// \brief Cost function on the state pace (optional).
    ///
    /// cost = _costfn(config)
    /// \param cost the cost of being in the current state
    typedef boost::function<dReal(const std::vector<dReal>&)> CostFn;
    CostFn _costfn;

    /** \brief Goal heuristic function.(optional)

        distance = _goalfn(config)

        Goal is complete when returns 0
        \param distance - distance to closest goal
     */
    typedef boost::function<dReal(const std::vector<dReal>&)> GoalFn;
    GoalFn _goalfn;

    /// \brief Distance metric between configuration spaces (optional)
    ///
    /// distmetric(config1,config2)
    ///
    /// Two configurations are considered the same when function returns 0.
    typedef boost::function<dReal(const std::vector<dReal>&, const std::vector<dReal>&)> DistMetricFn;
    DistMetricFn _distmetricfn;

    /** \brief Checks that all the constraints are satisfied between two configurations and passes in the velocity at each point.

        The simplest and most fundamental constraint is linearly interpolating the positions and velocities and checking constraints at each discrete point.

        errorcode = _checkpathvelocityconstraintsfn(q0, q1, dq0, dq1, timeelapsed, interval, options, configurations)

        When called, q0 and dq0 is guaranteed to be set on the robot.
        The function returns true if the path to q1 satisfies all the constraints of the planner.
        If q0==q1, and interval==IT_OpenStart or IT_OpenEnd, then only one configuration should be checked. It is recommended to use IT_OpenStart.
        Because this function can internally use neighstatefn, need to make sure that Q0->Q1 is going from initial to goal direction.

        \param q0 is the configuration the robot is coming from (shouldn't assume that it is currently set).
        \param q1 is the configuration the robot should move to.
        \param dq0 is the first time derivative (or velocity) of each DOF at q0.
        \param dq1 is the first time derivative (or velocity) of each DOF at q1.
        \param timeelapsed is the estimated time to go from q0 to q1 with the current constraints. Set to 0 if non-applicable.
        \param interval Specifies whether to check the end points of the interval for constraints
        \param options a mask of ConstraintFilterOptions
        \param filterreturn Optional argument that will hold the output information of the filter.
        \return \ref ConstraintFilterReturn::_returncode, which a combination of ConstraintFilterOptions
     */
    typedef boost::function<int (const std::vector<dReal>&, const std::vector<dReal>&, const std::vector<dReal>&, const std::vector<dReal>&, dReal, IntervalType, int, ConstraintFilterReturnPtr)> CheckPathVelocityConstraintFn;
    CheckPathVelocityConstraintFn _checkpathvelocityconstraintsfn;

    /// \brief wrapper function calling _checkpathvelocityconstraintsfn with some default args. Returns true if function doesn't exist.
    inline int CheckPathAllConstraints(const std::vector<dReal>& q0, const std::vector<dReal>& q1, const std::vector<dReal>& dq0, const std::vector<dReal>& dq1, dReal elapsedtime, IntervalType interval, int options=0xffff, ConstraintFilterReturnPtr filterreturn=ConstraintFilterReturnPtr()) const
    {
        if( !_checkpathvelocityconstraintsfn ) {
            return true;
        }
        return _checkpathvelocityconstraintsfn(q0, q1, dq0, dq1, elapsedtime, interval, options, filterreturn);
    }

    /** \brief Checks that all the constraints are satisfied at each discretized point between the two states (configuration, the first, and the second time derivatives).

       The two states are interpolated using a quintic polynomial

       errorcode = _checkpathvelocityaccelerationconstraintsfn(q0, q1, dq0, dq1, ddq0, ddq1, timeelapsed, interval, options, configurations)

       When called, q0 and dq0 is guaranteed to be set on the robot.
       The function returns true if the path to q1 satisfies all the constraints of the planner.

       \param q0 is the configuration the robot is coming from (shouldn't assume that it is currently set).
       \param q1 is the configuration the robot should move to.
       \param dq0 is the first time derivative (or velocity) of each DOF at q0.
       \param dq1 is the first time derivative (or velocity) of each DOF at q1.
       \param ddq0 is the second time derivative (or acceleration) of each DOF at q0.
       \param ddq1 is the second time derivative (or acceleration) of each DOF at q1.
       \param timeelapsed is the estimated time to go from q0 to q1 with the current constraints. Set to 0 if non-applicable.
       \param interval Specifies whether to check the end points of the interval for constraints
       \param options a mask of ConstraintFilterOptions
       \param filterreturn Optional argument that will hold the output information of the filter.
       \return \ref ConstraintFilterReturn::_returncode, which a combination of ConstraintFilterOptions
     */
    typedef boost::function<int (const std::vector<dReal>&, const std::vector<dReal>&, const std::vector<dReal>&, const std::vector<dReal>&, const std::vector<dReal>&, const std::vector<dReal>&, dReal, IntervalType, int, ConstraintFilterReturnPtr)> CheckPathVelocityAccelerationConstraintFn;
    CheckPathVelocityAccelerationConstraintFn _checkpathvelocityaccelerationconstraintsfn;

    /// \brief wrapper function calling _checkpathvelocityaccelerationconstraintsfn with some default args. Returns true if function doesn't exist.
    inline int CheckPathAllConstraints(const std::vector<dReal>& q0, const std::vector<dReal>& q1, const std::vector<dReal>& dq0, const std::vector<dReal>& dq1, const std::vector<dReal>& ddq0, const std::vector<dReal>& ddq1, dReal elapsedtime, IntervalType interval, int options=0xffff, ConstraintFilterReturnPtr filterreturn=ConstraintFilterReturnPtr()) const
    {
        if( !_checkpathvelocityaccelerationconstraintsfn ) {
            return true;
        }
        return _checkpathvelocityaccelerationconstraintsfn(q0, q1, dq0, dq1, ddq0, ddq1, elapsedtime, interval, options, filterreturn);
    }

    /** \brief Samples a random configuration (mandatory)

        The dimension of the returned sample is the dimension of the configuration space.
        success = samplefn(newsample)
     */
    typedef boost::function<bool (std::vector<dReal>&)> SampleFn;
    SampleFn _samplefn;

    /** \brief Samples a valid goal configuration (optional).

        If valid, the function should be called
        at every iteration. Any type of sampling probabilities and conditions can be encoded inside the function.
        The dimension of the returned sample is the dimension of the configuration space.
        success = samplegoalfn(newsample)
     */
    typedef boost::function<bool (std::vector<dReal>&)> SampleGoalFn;
    SampleGoalFn _samplegoalfn;

    /** \brief Samples a valid initial configuration (optional).

        If valid, the function should be called
        at every iteration. Any type of sampling probabilities and conditions can be encoded inside the function.
        The dimension of the returned sample is the dimension of the configuration space.
        success = sampleinitialfn(newsample)
     */
    typedef boost::function<bool (std::vector<dReal>&)> SampleInitialFn;
    SampleInitialFn _sampleinitialfn;

    /** \brief Returns a random configuration around a neighborhood (optional).

        _sampleneighfn(newsample,pCurSample,fRadius)

        \param pCurSample - the neighborhood to sample around
        \param  fRadius - specifies the max distance of sampling. The higher the value, the farther the samples will go
                          The distance metric can be arbitrary, but is usually PlannerParameters::pdistmetric.
        \return if sample was successfully generated return true, otherwise false
     */
    typedef boost::function<bool (std::vector<dReal>&, const std::vector<dReal>&, dReal)> SampleNeighFn;
    SampleNeighFn _sampleneighfn;

    /** \brief Sets the state values of the robot. Default is active robot joints (mandatory).

        int ret = _setstatevalues(values, options)

        \param values the array of values to set on the configuration space
        \param options user-defined options. default is 0
        \return If ret == 0, values were set with no problems. Otherwise a non-zero error code is returned.
     */
    typedef boost::function<int (const std::vector<dReal>&, int options)> SetStateValuesFn;
    SetStateValuesFn _setstatevaluesfn;

    /// \brief  calls _setstatevaluesfn. if it doesn't exist, tries calling the deprecated _setstatefn
    int SetStateValues(const std::vector<dReal>& values, int options=0) const;

    /// \brief Gets the state of the robot. Default is active robot joints (mandatory).
    typedef boost::function<void (std::vector<dReal>&)> GetStateFn;
    GetStateFn _getstatefn;

    /** \brief  Computes the difference of two states.

        _diffstatefn(q1,q2) -> q1 -= q2

        An explicit difference function is necessary for correct interpolation when there are circular joints.
        Default is regular subtraction.
     */
    typedef boost::function<void (std::vector<dReal>&,const std::vector<dReal>&)> DiffStateFn;
    DiffStateFn _diffstatefn;

    /** \brief Adds a delta state to a curent state, acting like a next-nearest-neighbor function along a given direction.

       status = _neighstatefn(q, qdelta, option) -> q = Filter(q + qdelta)

       \param q the current state. In order to save computation, assumes this state is the currently set configuration.
       \param qdelta the incremental configuration to be added to q
       \param options a set of flags from NeighborStateOptions
       \return one of NSS_X (NeighborStateStatus)

        In RRTs this is used for the extension operation. The new state is stored in the first parameter q.
        Note that the function can also add a filter to the final destination (like projecting onto a constraint manifold).
     */
    typedef OpenRAVE::NeighStateFn NeighStateFn;
    NeighStateFn _neighstatefn;

    /// to specify multiple initial or goal configurations, put them into the vector in series
    /// size always has to be a multiple of GetDOF()
    /// note: not all planners support multiple goals
    std::vector<dReal> vinitialconfig, vgoalconfig;

    /// \brief the initial velocities (at vinitialconfig) of the robot when starting to plan. If empty, then set to zero.
    std::vector<dReal> _vInitialConfigVelocities, _vGoalConfigVelocities;

    /// \brief the absolute limits of the configuration space.
    std::vector<dReal> _vConfigLowerLimit, _vConfigUpperLimit;

    /// \brief the absolute velocity limits of each DOF of the configuration space.
    std::vector<dReal> _vConfigVelocityLimit;

    /// \brief the absolute acceleration limits of each DOF of the configuration space.
    std::vector<dReal> _vConfigAccelerationLimit;

    /// \brief the absolute jerk limits of each DOF of the configuration space.
    std::vector<dReal> _vConfigJerkLimit;

    /// \brief the discretization resolution of each dimension of the configuration space
    std::vector<dReal> _vConfigResolution;

    /** \brief a discretization between the path that connects two configurations

        This length represents how dense the samples get distributed across the configuration space.
        It represents the maximum distance between neighbors when adding new configuraitons.
        If 0 or less, planner chooses best step length.
     */
    dReal _fStepLength;

    /// \brief maximum number of iterations before the planner gives up. If 0 or less, planner chooses best iterations.
    int _nMaxIterations;

    /// \brief max planning time in ms. If 0, then there is no time limit
    uint32_t _nMaxPlanningTime;

    /// \brief Specifies the planner that will perform the post-processing path smoothing before returning.
    ///
    /// If empty, will not path smooth the returned trajectories (used to measure algorithm time)
    std::string _sPostProcessingPlanner;

    /// \brief The serialized planner parameters to pass to the path optimizer.
    ///
    /// For example: std::stringstream(_sPostProcessingParameters) >> _parameters;
    std::string _sPostProcessingParameters;

    /// \brief Extra parameters data that does not fit within this planner parameters structure, but is still important not to lose all the information.
    std::string _sExtraParameters;

    /// \brief Random generator seed for all the random numbers a planner needs.
    ///
    /// The same seed should produce the smae results!
    uint32_t _nRandomGeneratorSeed;

    /// \brief Return the degrees of freedom of the planning configuration space
    virtual int GetDOF() const {
        return _configurationspecification.GetDOF();
    }

    /// \brief A list of samplers used in the callbacks of the planner parameter functions
    ///
    /// The list is useful when resetting the seeds for all the samplers
    /// For example, when _samplefn is set and a SpaceSampler is used as the underlying number generator, then it should be added to this list.
    std::list<SpaceSamplerBasePtr> _listInternalSamplers;

protected:
    // router to a default implementation of _checkpathconstraintsfn that calls on _checkpathvelocityconstraintsfn
    bool _CheckPathConstraintsOld(const std::vector<dReal>&q0, const std::vector<dReal>&q1, IntervalType interval, ConfigurationListPtr pvCheckedConfigurations) {
        RAVELOG_VERBOSE("using deprecated PlannerParameters::_checkpathconstraintsfn, consider switching to PlannerParameters::_checkpathvelocityconstraintsfn\n");
        if( !_checkpathvelocityconstraintsfn ) {
            return true;
        }

        ConstraintFilterReturnPtr filterreturn;
        std::vector<dReal> vzero(q0.size());
        if( !!pvCheckedConfigurations ) {
            pvCheckedConfigurations->resize(0);
            if( !filterreturn ) {
                filterreturn.reset(new ConstraintFilterReturn());
            }
            else {
                filterreturn->Clear();
            }
        }
        int ret = _checkpathvelocityconstraintsfn(q0, q1, vzero, vzero, 0, interval, 0xffff, filterreturn);
        if( !!filterreturn && filterreturn->_configurations.size() > 0 ) {
            size_t dof = q0.size();
            pvCheckedConfigurations->resize(filterreturn->_configurations.size()/dof);
            for(std::vector<dReal>::iterator it = filterreturn->_configurations.begin(); it != filterreturn->_configurations.end(); it += dof) {
                pvCheckedConfigurations->push_back(std::vector<dReal>(it, it+dof));
            }
        }
        return ret == 0;
    }

    inline boost::shared_ptr<PlannerParameters> shared_parameters() {
        return boost::static_pointer_cast<PlannerParameters>(shared_from_this());
    }
    inline boost::shared_ptr<PlannerParameters const > shared_parameters_const() const {
        return boost::static_pointer_cast<PlannerParameters const>(shared_from_this());
    }

    virtual bool SerializeXML(BaseXMLWriterPtr writer, int options) const override
    {
        return false;
    }

    /// \brief output the planner parameters in a string (in XML format)
    ///
    /// \param options if 1 will skip writing the extra parameters
    /// don't use PlannerParameters as a tag!
    virtual bool serialize(std::ostream& O, int options=0) const;

    //@{ XML parsing functions, parses the default parameters
    virtual ProcessElement startElement(const std::string& name, const AttributesList& atts) override;
    virtual bool endElement(const std::string& name) override;
    virtual void characters(const std::string& ch) override;
    std::stringstream _ss;         ///< holds the data read by characters
    boost::shared_ptr<std::stringstream> _sslocal;
    /// all the top-level XML parameter tags (lower case) that are handled by this parameter structure, should be registered in the constructor
    std::vector<std::string> _vXMLParameters;
    //@}

private:
    /// prevent copy constructors since it gets complicated with virtual functions
    PlannerParameters(const PlannerParameters &r);
    BaseXMLReaderPtr __pcurreader;         ///< temporary reader
    std::string __processingtag;
    int _plannerparametersdepth;

    /// outputs the data and surrounds it with \verbatim <PlannerParameters> \endverbatim tags
    friend OPENRAVE_API std::ostream& operator<<(std::ostream& O, const PlannerParameters& v);
    /// expects \verbatim <PlannerParameters> \endverbatim to be the first token. Parses stream until \verbatim </PlannerParameters> \endverbatim reached
    friend OPENRAVE_API std::istream& operator>>(std::istream& I, PlannerParameters& v);
};
typedef boost::shared_ptr<PlannerParameters> PlannerParametersPtr;
typedef boost::shared_ptr<PlannerParameters const> PlannerParametersConstPtr;
typedef boost::weak_ptr<PlannerParameters> PlannerParametersWeakPtr;
typedef boost::weak_ptr<PlannerParameters const> PlannerParametersWeakConstPtr;

/// \brief Planner error information
///
/// For constructors that have report, will internally creates a new pointer and store the contents of report
class OPENRAVE_API PlannerStatus
{
public:
    PlannerStatus();
    PlannerStatus(const uint32_t statusCode);
    PlannerStatus(const std::string& description, const uint32_t statusCode);
    PlannerStatus(const std::string& description, const uint32_t statusCode, CollisionReportPtr& report);
    PlannerStatus(const std::string& description, const uint32_t statusCode, const IkParameterization& ikparam);
    PlannerStatus(const std::string& description, const uint32_t statusCode, const IkParameterization& ikparam, CollisionReportPtr& report);
    PlannerStatus(const std::string& description, const uint32_t statusCode, const IkParameterization& ikparam, const std::vector<AccumulatorIndex>& vIkFailureInfoIndices);
    PlannerStatus(const std::string& description, const uint32_t statusCode, const std::vector<AccumulatorIndex>& vIkFailureInfoIndices);
    PlannerStatus(const std::string& description, const uint32_t statusCode, const std::vector<dReal>& jointValues);
    PlannerStatus(const std::string& description, const uint32_t statusCode, const std::vector<dReal>& jointValues, CollisionReportPtr& report);

    virtual ~PlannerStatus();

    PlannerStatus& SetErrorOrigin(const std::string& errorOrigin);
    PlannerStatus& SetPlannerParameters(PlannerParametersConstPtr parameters);

    void InitCollisionReport(CollisionReportPtr& collisionReport);
    void AddCollisionReport(const CollisionReport& collisionReport);
    void SaveToJson(rapidjson::Value& rPlannerStatus, rapidjson::Document::AllocatorType& alloc) const;

    inline uint32_t GetStatusCode() const {
        return statusCode;
    }

    /// \brief return true if planner gave a solution
    inline bool HasSolution() const {
        return statusCode&PS_HasSolution;
    }

    PlannerParametersConstPtr parameters;   ///< parameters used in the planner
    std::string description;                ///< Optional, the description of how/why the error happended. Displayed to the user by the UI. It will automatically be filled with a generic message corresponding to statusCode if not provided.
    uint32_t statusCode=0;                    // combination of PS_X fields (PlannerStatusCode)
    IkParameterization ikparam;             // Optional, the ik parameter that failed to find a solution.
    std::vector<dReal> jointValues;         // Optional, the robot's joint values in rad or m
    CollisionReportPtr report;              ///< Optional,  collision report at the time of the error. Ideally should contain contacts information.
    std::string errorOrigin;                // Auto, a string representing the code path of the error.
    std::vector<AccumulatorIndex> vIkFailureInfoIndices; ///< Optional, indices of the ikFailureInfos collected from the run.

    std::map< std::pair<std::string,std::string>, unsigned int > mCollidingLinksCount; // Counter for colliding body/link/geoms
    uint32_t numPlannerIterations=0; ///< number of planner iterations before failure
    uint64_t elapsedPlanningTimeUS=0; ///< us, elapsed time of the planner
};

#define OPENRAVE_PLANNER_STATUS(...) PlannerStatus(__VA_ARGS__).SetErrorOrigin(str(boost::format("[%s:%d %s] ")%OpenRAVE::RaveGetSourceFilename(__FILE__)%__LINE__%__FUNCTION__)).SetPlannerParameters(_parameters);
#define OPENRAVE_PLANNER_STATUS_NOPARAMS(...) PlannerStatus(__VA_ARGS__).SetErrorOrigin(str(boost::format("[%s:%d %s] ")%OpenRAVE::RaveGetSourceFilename(__FILE__)%__LINE__%__FUNCTION__));

/** \brief <b>[interface]</b> Planner interface that generates trajectories for target objects to follow through the environment. <b>If not specified, method is not multi-thread safe.</b> See \ref arch_planner.
    \ingroup interfaces
 */
class OPENRAVE_API PlannerBase : public InterfaceBase
{
public:
    typedef std::list< std::vector<dReal> > ConfigurationList;
    typedef boost::shared_ptr< PlannerBase::ConfigurationList > ConfigurationListPtr;
    typedef std::list< std::pair< std::vector<dReal>, std::vector<dReal> > > ConfigurationVelocityList;
    typedef boost::shared_ptr< PlannerBase::ConfigurationVelocityList > ConfigurationVelocityListPtr;

    // back compat
    typedef OpenRAVE::PlannerParameters PlannerParameters;
    typedef boost::shared_ptr<PlannerParameters> PlannerParametersPtr;
    typedef boost::shared_ptr<PlannerParameters const> PlannerParametersConstPtr;
    typedef boost::weak_ptr<PlannerParameters> PlannerParametersWeakPtr;
    typedef boost::weak_ptr<PlannerParameters const> PlannerParametersWeakConstPtr;

    /// \brief Planner progress information passed to each callback function
    class OPENRAVE_API PlannerProgress
    {
public:
        PlannerProgress();
        int _iteration;
    };

    PlannerBase(EnvironmentBasePtr penv);
    virtual ~PlannerBase() {

    }

    /// \return the static interface type this class points to (used for safe casting)
    static inline InterfaceType GetInterfaceTypeStatic() {
        return PT_Planner;
    }

    /** \brief Setup scene, robot, and properties of the plan, and reset all internal structures.

        \param robot main robot to be used for planning
        \param params The parameters of the planner, any class derived from PlannerParameters can be passed. The planner should copy these parameters for future instead of storing the pointer.
     */
    virtual PlannerStatus InitPlan(RobotBasePtr robot, PlannerParametersConstPtr params) = 0;

    /** \brief Setup scene, robot, and properties of the plan, and reset all structures with pparams.

        \param robot main robot to be used for planning
        \param isParameters The serialized form of the parameters. By default, this exists to allow third parties to
        pass information to planners without excplicitly knowning the format/internal structures used
        \return true if plan is initialized successfully and initial conditions are satisfied.
     */
    virtual PlannerStatus InitPlan(RobotBasePtr robot, std::istream& isParameters);

    /** \brief Executes the main planner trying to solve for the goal condition.

        Fill traj with the trajectory of the planned path that the robot needs to execute
        \param traj The output trajectory the robot has to follow in order to successfully complete the plan. If this planner is a path optimizer, the trajectory can be used as an input for generating a smoother path. The trajectory is for the configuration degrees of freedom defined by the planner parameters.
        \param planningoptions A set of PO_X options controlling planning and stauts
        \return the status that the planner returned in.
     */
    virtual PlannerStatus PlanPath(TrajectoryBasePtr traj, int planningoptions=0) = 0;

    /// \brief return the internal parameters of the planner
    virtual PlannerParametersConstPtr GetParameters() const = 0;

    /** \brief Callback function during planner execute

        \param progress planner progress information
     */
    typedef boost::function<PlannerAction(const PlannerProgress&)> PlanCallbackFn;

    /** \brief register a function that is called periodically during the plan loop.

        Allows the calling process to control the behavior of the planner from a high-level perspective
     */
    virtual UserDataPtr RegisterPlanCallback(const PlanCallbackFn& callbackfn);

    /// \brief sets the ik failure accumulator to use when running functions
    virtual void SetIkFailureAccumulator(IkFailureAccumulatorBasePtr& pIkFailureAccumulator);
        
protected:
    inline PlannerBasePtr shared_planner() {
        return boost::static_pointer_cast<PlannerBase>(shared_from_this());
    }
    inline PlannerBaseConstPtr shared_planner_const() const {
        return boost::static_pointer_cast<PlannerBase const>(shared_from_this());
    }

    /** \brief Calls a planner to optimizes the trajectory path.

        The PlannerParameters structure passed into the optimization planner is
        constructed with the same freespace constraints as this planner.
        This function should always be called in PlanPath to post-process the trajectory.
        \param probot the robot this trajectory is meant for, also uses the robot for checking collisions.
        \param traj Initial trajectory to be smoothed is inputted. If optimization path succeeds, final trajectory output is set in this variable. The trajectory is for the configuration degrees of freedom defined by the planner parameters.
     */
    virtual PlannerStatus _ProcessPostPlanners(RobotBasePtr probot, TrajectoryBasePtr traj);

    virtual bool _OptimizePath(RobotBasePtr probot, TrajectoryBasePtr traj) RAVE_DEPRECATED {
        return !!(_ProcessPostPlanners(probot,traj).statusCode & PS_HasSolution);
    }

    /// \brief Calls the registered callbacks in order and returns immediately when an action other than PA_None is returned.
    ///
    /// \param progress planner progress information
    virtual PlannerAction _CallCallbacks(const PlannerProgress& progress);

private:
    virtual const char* GetHash() const {
        return OPENRAVE_PLANNER_HASH;
    }

    std::list<UserDataWeakPtr> __listRegisteredCallbacks; ///< internally managed callbacks
    PlannerBasePtr __cachePostProcessPlanner; ///< cached version of the post process planner

    friend class CustomPlannerCallbackData;
};

OPENRAVE_API std::ostream& operator<<(std::ostream& O, const PlannerParameters& v);
OPENRAVE_API std::istream& operator>>(std::istream& I, PlannerParameters& v);

} // end namespace OpenRAVE

#endif
