package com.kuyue.file;

public class SequenceNum implements Sequence {
	
	private static ThreadLocal<Integer> numberContainer = new ThreadLocal<Integer>() {
		@Override
		protected Integer initialValue() {
			return 0;
		}
	};
	public int getNumber() {
		numberContainer.set(numberContainer.get() + 1);
		return numberContainer.get();
	}

}
