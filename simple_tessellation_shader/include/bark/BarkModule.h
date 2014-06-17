#pragma once
class BarkModule {
	public:
		BarkModule(void);
		BarkModule(float, float, float);
		~BarkModule(void);

		float	getRestLength() const;
		float	getExtension()	const;
		float	getStiffness()	const;
		
		virtual unsigned	type() const = 0;
		virtual bool		solveStress() const = 0;

		void extend(float);

	protected:
		float	_restLength;
		float	_extension;
		float	_stiffness;
};

